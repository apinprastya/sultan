/*
 * sellpriceactions.cpp
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
#include "sellpriceactions.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"

using namespace LibServer;
using namespace LibDB;

SellPriceActions::SellPriceActions():
    ServerAction("sellprices", "id")
{
    mFlag = HAS_UPDATE_FIELD | USE_TRANSACTION;
}

LibG::Message SellPriceActions::insert(LibG::Message *msg)
{
    LibG::Message message(msg);
    const QString &barcode = msg->data("barcode").toString();
    const float &count = msg->data("count").toFloat();
    DbResult res = mDb->where("barcode = ", barcode)->where("count = ", count)->get(mTableName);
    if(!res.isEmpty()) {
        message.setError(QObject::tr("Item with count already exist"));
        return message;
    }
    return ServerAction::insert(msg);
}

LibG::Message SellPriceActions::update(LibG::Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->where("id = ", msg->data("id").toInt())->get(mTableName);
    if(res.isEmpty()) {
        message.setError(QObject::tr("Item with id not found"));
        return message;
    } else {
        const QVariantMap &oldData = res.first();
        const QVariantMap &newData = msg->data("data").toMap();
        if(newData.contains("count") && oldData["count"].toFloat() != newData["count"].toFloat()) {
            res = mDb->where("barcode = ", oldData["barcode"])->where("count = ", newData["count"])->get(mTableName);
            if(!res.isEmpty()) {
                message.setError(QObject::tr("Item with count already exist"));
                return message;
            }
        }
    }
    return ServerAction::update(msg);
}

void SellPriceActions::selectAndJoin()
{
    mDb->table(mTableName)->select("sellprices.*, items.name")->join("LEFT JOIN items ON sellprices.barcode = items.barcode");
}

QMap<QString, QString> SellPriceActions::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("barcode", "sellprices.barcode");
    return map;
}
