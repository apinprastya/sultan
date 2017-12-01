/*
 *
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

#include "checkstockaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "global_constant.h"
#include "util.h"
#include "util/itemutil.h"
#include <QStringBuilder>
#include <QDateTime>
#include <QDebug>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

CheckStockAction::CheckStockAction():
    ServerAction("checkstocks", "id")
{
    mFlag = AFTER_INSERT | USE_TRANSACTION;
}

LibG::Message CheckStockAction::insert(LibG::Message *msg)
{
    LibG::Message message(msg);
    if(hasFlag(HAS_UPDATE_FIELD))
        msg->addData("updated_at", QDateTime::currentDateTime());
    /*bool hasName = msg->hasData("category_id");
    QVariantMap item{{"name", msg->data("name")}, {"category_id", msg->takeData("category_id")},
                    {"suplier_id", msg->takeData("suplier_id")}, {"buy_price", msg->takeData("buy_price")}};
    double sellPrice = msg->takeData("sell_price").toDouble();*/
    if(hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction())
        mDb->beginTransaction();
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res = mDb->where("id = ", mDb->lastInsertedId())->get(mTableName);
        const QVariantMap d = res.first();
        /*if(hasName) {
            mDb->where("barcode = ", msg->data("barcode"));
            if(mDb->update("items", item)) {
                res = mDb->where("count = ", 1)->where("barcode = ", msg->data("barcode"))->get("sellprices");
                if(res.isEmpty()) {
                    QVariantMap sp{{"barcode", item["barcode"]}, {"count", 1}, {"price", sellPrice}};
                    mDb->insert("sellprices", sp);
                } else {
                    mDb->where("id = ", res.first()["id"]);
                    mDb->update("sellprices", QVariantMap{{"price", sellPrice}});
                }
            }
        }*/
        if(hasFlag(AFTER_INSERT)) afterInsert(d);
        message.setData(d);
    }
    if(hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction()) {
        if(!mDb->commit()) {
            mDb->roolback();
            message.setError(mDb->lastError().text());
        }
    }
    return message;
}

void CheckStockAction::afterInsert(const QVariantMap &data)
{
    /*mDb->where("barcode = ", data["barcode"]);
    QVariantMap d{{"stock", data["real_stock"]}};
    mDb->update("items", d);*/
    float diff = data["real_stock"].toFloat() - data["system_stock"].toFloat();
    ItemUtil util(mDb);
    util.insertStock(data["barcode"].toString(), QString("Checkstock %1").arg(data["barcode"].toString()),
            STOCK_CARD_TYPE::CHECKSTOCK, diff, 0, QVariantMap());
}
