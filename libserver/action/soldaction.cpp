/*
 * soldaction.cpp
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
#include "soldaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "util/itemutil.h"
#include <QDateTime>
#include <QDebug>
#include <QStringBuilder>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

// NOTE : this flag is taken from CashierItem::Flag
#define ITEM (0x1)
#define SERVICE (0x2)
#define RETURN (1 << 8)
#define RETURNED (1 << 9)

SoldAction::SoldAction() : ServerAction("solds", "id") {
    mFlag = USE_TRANSACTION;
    mFunctionMap.insert(MSG_COMMAND::NEW_SOLD, std::bind(&SoldAction::insertSold, this, std::placeholders::_1));
}

Message SoldAction::insertSold(Message *msg) {
    LibG::Message message(msg);
    const QVariantList &l = msg->data("cart").toList();
    // auto now = QDateTime::currentMSecsSinceEpoch() / 10000;
    double total = msg->data("total").toDouble();
    double payment = msg->data("payment").toDouble();
    int cust_id = msg->data("customer_id").toInt();
    int user_id = msg->data("user_id").toInt();
    int poin = msg->data("reward").toInt();
    msg->removeData("cart");
    if (mDb->isSupportTransaction())
        mDb->beginTransaction();
    if (mDb->insert(mTableName, msg->data())) {
        QVariant id = mDb->lastInsertedId();
        for (auto v : l) {
            QVariantMap m = v.toMap();
            m["sold_id"] = id;
            /*DbResult res = mDb->where("barcode = ", m["barcode"])->get("items");
            const QVariantMap &item = res.first();
            if((flag & RETURN) != 0) {
                mDb->exec(QString("UPDATE solditems SET flag = flag | %1 WHERE id =
            %2").arg(RETURNED).arg(m["link_id"].toInt()));
                //TODO: need to check if it a FIFO operation in the future
                mDb->exec(QString("UPDATE items SET buy_price = (buy_price + %1) / (stock - %2) WHERE barcode = %3").
                          arg(m["buy_price"].toDouble()).arg(m["count"].toFloat()).arg(m["barcode"].toString()));
            } else {
                m["buy_price"] = m["count"].toFloat() * item["buy_price"].toDouble();
            }
            mDb->insert("solditems", m);
            if((itemflag & ITEM_FLAG::CALCULATE_STOCK) != 0) {
                mDb->exec(QString("UPDATE items SET stock = stock - %1 WHERE barcode =
            %2").arg(m["count"].toFloat()).arg(m["barcode"].toString()));
            }*/
            ItemUtil iu(mDb);
            iu.insertStock(m["barcode"].toString(), msg->data("number").toString(), STOCK_CARD_TYPE::SOLD,
                           -m["count"].toFloat(), 0, m);
        }
        // transaction
        double mon = payment > total ? total : payment;
        QVariantMap tr{{"date", QDateTime::currentDateTime()},
                       {"number", msg->data("number")},
                       {"type", TRANSACTION_TYPE::INCOME},
                       {"link_id", id},
                       {"link_type", TRANSACTION_LINK_TYPE::SOLD},
                       {"user_id", user_id},
                       {"machine_id", msg->data("machine_id")},
                       {"transaction_total", total},
                       //{"customer_id", cust_id},
                       {"bank_id", msg->data("bank_id")},
                       {"money_total", mon},
                       {"detail", QObject::tr("Sold : %1").arg(msg->data("number").toString())}};
        mDb->insert("transactions", tr);
        // credit to customer
        if (payment < total) {
            QVariantMap cr;
            cr.insert("customer_id", cust_id);
            cr.insert("number", msg->data("number"));
            cr.insert("link_id", id);
            cr.insert("detail", QObject::tr("Credit from transaction %1").arg(msg->data("number").toString()));
            cr.insert("credit", total - payment);
            cr.insert("machine_id", msg->data("machine_id"));
            cr.insert("user_id", user_id);
            mDb->insert("customercredits", cr);
            mDb->exec(QString("UPDATE customers SET credit = (SELECT SUM(credit) FROM customercredits WHERE "
                              "customer_id = %1) WHERE id = %2")
                          .arg(cust_id)
                          .arg(cust_id));
        }
        // Reward poin
        if (cust_id > 0) {
            QVariantMap p;
            p.insert("customer_id", cust_id);
            p.insert("number", msg->data("number"));
            p.insert("link_id", id);
            p.insert("detail", QObject::tr("Poin from transaction %1").arg(msg->data("number").toString()));
            p.insert("reward", poin);
            p.insert("total_shop", total);
            p.insert("user_id", user_id);
            mDb->insert("customerrewards", p);
            mDb->exec(QString("UPDATE customers SET reward = (SELECT SUM(reward) FROM customerrewards WHERE "
                              "customer_id = %1) WHERE id = %2")
                          .arg(cust_id)
                          .arg(cust_id));
        }
        if (mDb->isSupportTransaction()) {
            if (!mDb->commit()) {
                message.setError(mDb->lastError().text());
                return message;
            }
        }
        msg->setData(QVariantMap{{"id", id}});
        return get(msg);
    } else {
        message.setError(mDb->lastError().text());
    }
    return message;
}

Message SoldAction::get(Message *msg) {
    LibG::Message message(msg);
    DbResult res = mDb->where("id = ", msg->data("id"))->get(mTableName);
    if (!res.isEmpty()) {
        message.setData(res.first());
        int cust_id = message.data("customer_id").toInt();
        res = mDb->where("sold_id = ", msg->data("id"))->get("solditems");
        message.addData("cart", res.data());
        if (cust_id > 0) {
            res = mDb->where("id = ", cust_id)->get("customers");
            if (!res.isEmpty())
                message.addData("customer", res.first());
        }
    } else {
        message.setError("Data not found");
    }
    return message;
}

void SoldAction::calculateStock(const QVariantMap &data) {
    int flag = data["flag"].toInt();
    if ((flag & ITEM_FLAG::CALCULATE_STOCK) != 0) {
        // mDb->exec(QString("UPDATE items SET stock = stock - %1 WHERE barcode =
        // %2").arg(m["count"].toFloat()).arg(m["barcode"].toString())); if((flag & ITEM_FLAG))
        // TODO : apply semua yang dibawahnya dan diatasnya
    }
}
