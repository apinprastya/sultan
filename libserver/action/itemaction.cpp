/*
 * itemaction.cpp
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
#include "itemaction.h"
#include "global_constant.h"
#include "db.h"
#include "queryhelper.h"
#include <QStringBuilder>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ItemAction::ItemAction():
    ServerAction("items", "barcode")
{
    mFunctionMap.insert(MSG_COMMAND::CASHIER_PRICE, std::bind(&ItemAction::prices, this, std::placeholders::_1));
}

Message ItemAction::insert(Message *msg)
{
    LibG::Message message(msg);
    const double sellprice = msg->data("sell_price").toDouble();
    msg->removeData("sell_price");
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res = mDb->where("id = ", mDb->lastInsertedId())->get(mTableName);
        message.setData(res.first());
        QVariantMap d;
        d.insert("barcode", msg->data("barcode"));
        d.insert("count", 1);
        d.insert("price", sellprice);
        mDb->insert("sellprices", d);
    }
    return message;
}

Message ItemAction::update(Message *msg)
{
    LibG::Message message(msg);
    QVariantMap data = msg->data("data").toMap();
    const double sellprice = data["sell_price"].toDouble();
    data.remove("sell_price");
    mDb->where(mIdField % " = ", msg->data(mIdField));
    if(!mDb->update(mTableName, data)) {
        message.setError(mDb->lastError().text());
    } else {
        QVariantMap d;
        d.insert("price", sellprice);
        mDb->where("barcode = ", msg->data("barcode"))->where("count = '1.0'")->update("sellprices", d);
        DbResult res = mDb->where("id = ", msg->data("id"))->get(mTableName);
        message.setData(res.first());
    }
    return message;
}

LibG::Message ItemAction::prices(LibG::Message *msg)
{
    LibG::Message message(msg);
    const QString &barcode = msg->data("barcode").toString();
    //get item detail
    mDb->table(mTableName)->where("barcode = ", barcode);
    DbResult res = mDb->exec();
    if(res.isEmpty()) {
        message.setError("Item not found");
    } else {
        message.addData("item", res.first());
        mDb->table("sellprices")->where("barcode = ", barcode);
        res = mDb->exec();
        if(res.isEmpty()) {
            message.setError("Item has no price");
        } else {
            message.addData("prices", res.data());
        }
    }
    return message;
}

void ItemAction::selectAndJoin()
{
    mDb->select(mTableName % ".*, supliers.name as suplier, categories.name as category, \
                (select price from sellprices where barcode = items.barcode order by count asc limit 1) as sell_price")->
            join("LEFT JOIN supliers ON supliers.id = items.suplier_id")->
                join("LEFT JOIN categories ON categories.id = items.category_id");
}

QMap<QString, QString> ItemAction::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("name", "items.name");
    map.insert("suplier", "supliers.name");
    return map;
}
