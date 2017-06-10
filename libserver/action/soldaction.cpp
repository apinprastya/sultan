/*
 * soldaction.cpp
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
#include "soldaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include <QDateTime>
#include <QStringBuilder>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

static int NEXT_VAL = 0;

SoldAction::SoldAction():
    ServerAction("solds", "id")
{
    mFunctionMap.insert(MSG_COMMAND::NEW_SOLD, std::bind(&SoldAction::insertSold, this, std::placeholders::_1));
}

Message SoldAction::insertSold(Message *msg)
{
    LibG::Message message(msg);
    const QVariantList &l = msg->data("cart").toList();
    auto now = QDateTime::currentMSecsSinceEpoch() / 10000;
    double total = msg->data("total").toDouble();
    double payment = msg->data("payment").toDouble();
    int cust_id = msg->data("customer_id").toInt();
    msg->removeData("cart");
    QString number = QString("%1-%2").arg(now).arg(NEXT_VAL++, 3, 16, QChar('0'));
    msg->addData("number", number);
    if(mDb->isSupportTransaction()) mDb->beginTransaction();
    if(mDb->insert(mTableName, msg->data())) {
        //TODO: change it to insert batch
        QVariant id = mDb->lastInsertedId();
        for(auto v : l) {
            QVariantMap m = v.toMap();
            m["sold_id"] = id;
            DbResult res = mDb->where("barcode = ", m["barcode"])->get("items");
            m["buy_price"] = m["count"].toFloat() * res.first()["buy_price"].toDouble();
            mDb->insert("solditems", m);
            mDb->exec(QString("UPDATE items SET stock = stock - %1 WHERE barcode = %2").arg(m["count"].toFloat()).arg(m["barcode"].toString()));
        }
        msg->setData(QVariantMap{{"id", id}});
        //credit to customer
        if(payment < total) {
            QVariantMap cr;
            cr.insert("customer_id", cust_id);
            cr.insert("number", number);
            cr.insert("link_id", id);
            cr.insert("detail", QObject::tr("Credit from transaction %1").arg(number));
            cr.insert("credit", total - payment);
            mDb->insert("customer_credits", cr);
            mDb->exec(QString("UPDATE customers SET credit = (SELECT SUM(credit) FROM customer_credits WHERE customer_id = %1) WHERE id = %2").arg(cust_id).arg(cust_id));
        }
        //TODO : calculate the customer reward
        if(mDb->isSupportTransaction()) {
            if(!mDb->commit()) {
                message.setError(mDb->lastError().text());
                return message;
            }
        }
        return get(msg);
    } else {
        message.setError(mDb->lastError().text());
    }
    return message;
}

Message SoldAction::get(Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->where("id = ", msg->data("id"))->get(mTableName);
    if(!res.isEmpty()) {
        message.setData(res.first());
        res = mDb->where("sold_id = ", msg->data("id"))->get("solditems");
        message.addData("cart", res.data());
    } else {
        message.setError("Data not found");
    }
    return message;
}
