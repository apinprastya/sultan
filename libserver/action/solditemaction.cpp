/*
 * solditemaction.cpp
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
#include "solditemaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include <QStringBuilder>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

SoldItemAction::SoldItemAction():
    ServerAction("solditems", "id")
{
    mFlag = USE_TRANSACTION;
    mFunctionMap.insert(MSG_COMMAND::SOLD_SUMMARY, std::bind(&SoldItemAction::getSummary, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::SOLD_ITEM_REPORT, std::bind(&SoldItemAction::report, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&SoldItemAction::exportData, this, std::placeholders::_1));
}

void SoldItemAction::selectAndJoin()
{
    mDb->select("*, (final - buy_price) as margin");
}

Message SoldItemAction::getSummary(Message *msg)
{
    LibG::Message message(msg);
    mDb->table(mTableName);
    mDb->select("sum(final) as total, sum(final - buy_price) as margin");
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    message.setData(mDb->exec().first());
    return message;
}

Message SoldItemAction::report(Message *msg)
{
    LibG::Message message(msg);
    mDb->table(mTableName)->select("sum(solditems.count) as count, solditems.barcode, items.name, items.stock, items.unit, categories.name as category, supliers.name as suplier")->
            join("LEFT JOIN items ON items.barcode = solditems.barcode")->
            join("LEFT JOIN supliers ON supliers.id = items.suplier_id")->
            join("LEFT JOIN categories ON categories.id = items.category_id")->group("solditems.barcode")->sort("count DESC");
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    if(!(msg->data().contains(QStringLiteral("start")) && msg->data().value(QStringLiteral("start")).toInt() > 0))
        message.addData(QStringLiteral("total"),
                        mDb->clone()->reset()->table("(" + mDb->getSelectQuery() + ") d")->count());
    setStart(&message, msg);
    mDb = QueryHelper::limitOffset(mDb, msg->data());
    mDb = QueryHelper::sort(mDb, msg->data());
    message.addData(QStringLiteral("data"), mDb->exec().data());
    return message;
}

Message SoldItemAction::exportData(Message *msg)
{
    LibG::Message message(msg);
    QString arr;
    arr.append("date;barcode;name;count;price;discount;total;buy_price;margin;\n");
    mDb->table(mTableName);
    selectAndJoin();
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    mDb = QueryHelper::sort(mDb, msg->data());
    const int limit = 500;
    int start = 0;
    while(true) {
        DbResult res = mDb->clone()->start(start)->limit(limit)->exec();
        if(res.isEmpty()) break;
        for(int i = 0; i < res.size(); i++) {
            const QVariantMap &d = res.data(i);
            double margin = d["total"].toDouble() - d["buy_price"].toDouble();
            arr.append(d["created_at"].toString() % ";");
            arr.append(d["barcode"].toString() % ";");
            arr.append(d["name"].toString() % ";");
            arr.append(d["count"].toString() % ";");
            arr.append(d["price"].toString() % ";");
            arr.append(d["discount"].toString() % ";");
            arr.append(d["final"].toString() % ";");
            arr.append(d["buy_price"].toString() % ";");
            arr.append(QString::number(margin) % ";\n");
        }
        start += limit;
    }
    message.addData("data", arr);
    return message;
}
