/*
 * purchaseaction.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "global_constant.h"
#include "db.h"
#include <QDateTime>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

PurchaseAction::PurchaseAction():
    ServerAction("purchases", "id")
{
    mFlag = HAS_UPDATE_FIELD | AFTER_INSERT | AFTER_UPDATE;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&PurchaseAction::summary, this, std::placeholders::_1));
}

LibG::Message PurchaseAction::del(LibG::Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->where("purchase_id = ", msg->data(mIdField))->get("purchaseitems");
    if(!res.isEmpty()) {
        message.setError("Purchase item must empty before deleted!");
        return message;
    }
    mDb->where("id = ", msg->data(mIdField));
    if(!mDb->del(mTableName)) {
        message.setError(mDb->lastError().text());
    }
    return message;
}

Message PurchaseAction::summary(Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->select("sum(final) as total")->where("payment_type = ", PURCHASEPAYMENT::TEMPO)->
            where("status = ", PAYMENT_STATUS::UNPAID)->get(mTableName);
    message.setData(res.first());
    return message;
}

void PurchaseAction::afterInsert(const QVariantMap &data)
{
    if(data["payment_type"].toInt() == PURCHASEPAYMENT::DIRECT) {
        insertTransaction(data);
    }
}

void PurchaseAction::afterUpdate(const QVariantMap &/*oldData*/, const QVariantMap &data)
{
    DbResult res = mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)->where("link_id = ", data["id"])->get("transactions");
    if(res.isEmpty()) {
        if(data["status"].toInt() == PAYMENT_STATUS::PAID) {
            insertTransaction(data);
        }
    } else {
        int type = data["payment_type"].toInt();
        if(type == PURCHASEPAYMENT::TEMPO && data["status"].toInt() == PAYMENT_STATUS::UNPAID) {
            mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)->where("link_id = ", data["id"])->del("transactions");
        } else {
            updateTransaction(data);
        }
    }
}

void PurchaseAction::selectAndJoin()
{
    mDb->table(mTableName)->select("purchases.*, supliers.name as suplier")->
            join("LEFT JOIN supliers ON purchases.suplier_id = supliers.id");
}

QMap<QString, QString> PurchaseAction::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("suplier", "supliers.name");
    return map;
}

void PurchaseAction::insertTransaction(const QVariantMap &data)
{
    QVariantMap d{{"date", QDateTime::currentDateTime()}, {"number", data["number"]},
                 {"type", TRANSACTION_TYPE::EXPENSE}, {"link_id", data["id"]},
                 {"link_type", TRANSACTION_LINK_TYPE::PURCHASE}, {"transaction_total", data["final"]},
                 {"user_id", data["user_id"]}, {"machine_id", data["machine_id"]},
                 {"bank_id", data["bank_id"]}, {"money_total", data["final"]},
                 {"detail", QObject::tr("Purchase : %1").arg(data["number"].toString())}};
    if(data["payment_type"].toInt() == PURCHASEPAYMENT::TEMPO)
        d["date"] = data["payment_date"];
    mDb->insert("transactions", d);
}

void PurchaseAction::updateTransaction(const QVariantMap &data)
{
    mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)->where("link_id = ", data["id"]);
    QVariantMap d{{"transaction_total", data["final"]}, {"money_total", data["final"]},
                 {"number", data["number"]}, {"detail", QObject::tr("Purchase : %1").arg(data["number"].toString())}};
    if(data["payment_type"].toInt() == PURCHASEPAYMENT::TEMPO)
        d["date"] = data["payment_date"];
    mDb->update("transactions", d);
}
