/*
 * purchaseaction.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Sultan.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "purchaseaction.h"
#include "db.h"
#include "global_constant.h"
#include <QDateTime>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

PurchaseAction::PurchaseAction() : ServerAction("purchases", "id") {
    mFlag = HAS_UPDATE_FIELD | AFTER_INSERT | AFTER_UPDATE | USE_TRANSACTION | BEFORE_DELETE | BEFORE_INSERT;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&PurchaseAction::summary, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::ADD_PAYMENT, std::bind(&PurchaseAction::addPayment, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::UPDATE_PAYMENT,
                        std::bind(&PurchaseAction::updatePayment, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::DELETE_PAYMENT,
                        std::bind(&PurchaseAction::deletePayment, this, std::placeholders::_1));
}

Message PurchaseAction::summary(Message *msg) {
    LibG::Message message(msg);
    DbResult res = mDb->select("sum(final) as total")
                       ->where("payment_type = ", PURCHASEPAYMENT::TEMPO)
                       ->where("status = ", PAYMENT_STATUS::UNPAID)
                       ->get(mTableName);
    message.setData(res.first());
    return message;
}

bool PurchaseAction::beforeInsert(const QVariantMap &data, Message *retMsg) {
    DbResult res = mDb->where("number = ", data["number"].toString())->get(mTableName);
    if (!res.isEmpty()) {
        retMsg->setError(QObject::tr("Purchase with number already exist!"));
        return false;
    }
    return true;
}

void PurchaseAction::afterInsert(const QVariantMap &data) {
    if (data["payment_type"].toInt() == PURCHASEPAYMENT::DIRECT) {
        insertTransaction(data);
    }
}

void PurchaseAction::afterUpdate(const QVariantMap & /*oldData*/, const QVariantMap &data) {
    DbResult res = mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)
                       ->where("link_id = ", data["id"])
                       ->get("transactions");
    if (res.isEmpty()) {
        if (data["status"].toInt() == PAYMENT_STATUS::PAID) {
            insertTransaction(data);
        }
    } else {
        int type = data["payment_type"].toInt();
        if (type == PURCHASEPAYMENT::TEMPO && data["status"].toInt() == PAYMENT_STATUS::UNPAID) {
            mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)
                ->where("link_id = ", data["id"])
                ->del("transactions");
        } else {
            updateTransaction(data);
        }
    }
}

bool PurchaseAction::beforeDelete(const QVariantMap &oldData, Message *retMsg) {
    DbResult res = mDb->where("purchase_id = ", oldData["id"])->get("purchaseitems");
    if (!res.isEmpty()) {
        retMsg->setError("Purchase item must empty before deleted!");
        return false;
    }
    return true;
}

void PurchaseAction::selectAndJoin() {
    mDb->table(mTableName)
        ->select("purchases.*, supliers.name as suplier")
        ->join("LEFT JOIN supliers ON purchases.suplier_id = supliers.id");
}

QMap<QString, QString> PurchaseAction::fieldMap() const {
    QMap<QString, QString> map;
    map.insert("suplier", "supliers.name");
    return map;
}

void PurchaseAction::insertTransaction(const QVariantMap &data) {
    QVariantMap d{{"date", QDateTime::currentDateTime()},
                  {"number", data["number"]},
                  {"type", TRANSACTION_TYPE::EXPENSE},
                  {"link_id", data["id"]},
                  {"link_type", TRANSACTION_LINK_TYPE::PURCHASE},
                  {"transaction_total", -data["final"].toDouble()},
                  {"user_id", data["user_id"]},
                  {"machine_id", data["machine_id"]},
                  {"bank_id", data["bank_id"]},
                  {"money_total", -data["final"].toDouble()},
                  {"detail", QObject::tr("Purchase : %1").arg(data["number"].toString())}};
    if (data["payment_type"].toInt() == PURCHASEPAYMENT::TEMPO)
        d["date"] = data["payment_date"];
    mDb->insert("transactions", d);
}

void PurchaseAction::updateTransaction(const QVariantMap &data) {
    mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)->where("link_id = ", data["id"]);
    QVariantMap d{{"transaction_total", -data["final"].toDouble()},
                  {"money_total", -data["final"].toDouble()},
                  {"number", data["number"]},
                  {"detail", QObject::tr("Purchase : %1").arg(data["number"].toString())}};
    if (data["payment_type"].toInt() == PURCHASEPAYMENT::TEMPO)
        d["date"] = data["payment_date"];
    mDb->update("transactions", d);
}

double PurchaseAction::totalPayment(int id) {
    DbResult paymentDbResult = mDb->select("COALESCE(sum(money_total), 0) as payment")
                                   ->where("link_id = ", id)
                                   ->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)
                                   ->get("transactions");
    return paymentDbResult.first()["payment"].toDouble();
}

Message PurchaseAction::addPayment(Message *msg) {
    LibG::Message message(msg);

    const auto &data = msg->data();
    auto linkId = data["link_id"].toInt();

    DbResult res = mDb->where("id = ", linkId)->get("purchases");
    bool paymentDone = false;
    if (!res.isEmpty()) {
        double total = res.first()["final"].toDouble();
        double received = totalPayment(linkId);
        double residual = total + received;
        double moneyTotal = -msg->data("money_total").toDouble();
        paymentDone = qFuzzyCompare(residual, moneyTotal);
        if (moneyTotal > residual) {
            message.setError("payment_too_big");
            return message;
        }
        mDb->insert("transactions", data);
    }

    mDb->where("id = ", linkId)
        ->update("purchases", QVariantMap{{"status", paymentDone ? PAYMENT_STATUS::PAID : PAYMENT_STATUS::UNPAID}});

    return message;
}

Message PurchaseAction::deletePayment(LibG::Message *msg) {
    LibG::Message message(msg);
    int id = msg->data("id").toInt();
    int purchaseId = msg->data("purchaseId").toInt();
    mDb->where("id = ", id)->del("transactions");
    double received = totalPayment(purchaseId);
    DbResult purchaseResult = mDb->where("id = ", purchaseId)->get("purchases");
    if (!purchaseResult.isEmpty()) {
        double total = purchaseResult.first()["final"].toDouble();
        bool paymentDone = qFuzzyCompare(received, total) || received > total;
        mDb->where("id = ", purchaseId)
            ->update("purchases", QVariantMap{{"status", paymentDone ? PAYMENT_STATUS::PAID : PAYMENT_STATUS::UNPAID}});
    }

    return message;
}

Message PurchaseAction::updatePayment(LibG::Message *msg) {
    LibG::Message message(msg);
    int id = msg->data("id").toInt();
    int purchaseId = msg->data("purchaseId").toInt();
    mDb->where("id = ", id)->update("transactions", msg->data("data").toMap());
    double received = totalPayment(purchaseId);
    DbResult purchaseResult = mDb->where("id = ", purchaseId)->get("purchases");
    if (!purchaseResult.isEmpty()) {
        double total = purchaseResult.first()["final"].toDouble();
        bool paymentDone = qFuzzyCompare(received, total) || received > total;
        mDb->where("id = ", purchaseId)
            ->update("purchases", QVariantMap{{"status", paymentDone ? PAYMENT_STATUS::PAID : PAYMENT_STATUS::UNPAID}});
    }
    return message;
}
