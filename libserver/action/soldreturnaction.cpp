/*
 * soldreturnaction.cpp
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
#include "soldreturnaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "util/itemutil.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

SoldReturnAction::SoldReturnAction():
    ServerAction("solditemreturns", "id")
{
    mFlag = USE_TRANSACTION | AFTER_INSERT | AFTER_DELETE | AFTER_UPDATE;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&SoldReturnAction::summary, this, std::placeholders::_1));
}

LibG::Message SoldReturnAction::summary(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->table(mTableName);
    mDb->select("sum(total) as total")->where("status =", 0);
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    message.setData(mDb->exec().first());
    return message;
}

void SoldReturnAction::selectAndJoin()
{
    mDb->select("solditemreturns.*, solds.number as sold_number, solds.created_at as sold_created_at")->
            join("LEFT JOIN solds ON solditemreturns.sold_id = solds.id");
}

void SoldReturnAction::afterInsert(const QVariantMap &data)
{
    int status = data["status"].toInt();
    const QString &barcode = data["barcode"].toString();
    ItemUtil util(mDb);
    util.insertStock(barcode, QObject::tr("Sold Return : %1").arg(barcode), STOCK_CARD_TYPE::SOLD_RETURN, data["count"].toFloat() - data["item_returned"].toFloat(), 0, data["id"].toInt());
    if(status == PURCHASE_RETURN_STATUS::RETURNED) {
        QVariantMap d{{"date", data["return_date"]}, {"number", data["barcode"]},
                     {"type", TRANSACTION_TYPE::EXPENSE}, {"link_id", data["id"]},
                     {"link_type", TRANSACTION_LINK_TYPE::SOLD_RETURN}, {"transaction_total", -data["money_returned"].toDouble()},
                     {"user_id", data["user_id"]}, {"machine_id", data["machine_id"]},
                     {"bank_id", data["bank_id"]}, {"money_total", -data["money_returned"].toDouble()},
                     {"detail", QObject::tr("Sold Return : %1").arg(data["barcode"].toString())}};
        mDb->insert("transactions", d);
    }
}

void SoldReturnAction::afterUpdate(const QVariantMap &oldData, const QVariantMap &newData)
{
    float addCount = 0;
    const float &oldCount = oldData["count"].toFloat();
    const float &newCount = newData["count"].toFloat();
    const float &oldRet = oldData["item_returned"].toFloat();
    const float &newRet = newData["item_returned"].toFloat();
    const int oldStatus = oldData["status"].toInt();
    const int newStatus = newData["status"].toInt();
    if(oldCount != newCount) addCount = oldCount - newCount;
    if(oldStatus != newStatus) {
        if(newStatus == PURCHASE_RETURN_STATUS::UNRETURN) {
            addCount -= oldRet;
            mDb->where("link_type =", TRANSACTION_LINK_TYPE::SOLD_RETURN)->where("link_id =", newData["id"])->del("transactions");
        } else {
            addCount += newRet;
            QVariantMap d{{"date", newData["return_date"]}, {"number", newData["barcode"]},
                         {"type", TRANSACTION_TYPE::EXPENSE}, {"link_id", newData["id"]},
                         {"link_type", TRANSACTION_LINK_TYPE::SOLD_RETURN}, {"transaction_total", -newData["money_returned"].toDouble()},
                         {"user_id", newData["user_id"]}, {"machine_id", newData["machine_id"]},
                         {"bank_id", newData["bank_id"]}, {"money_total", -newData["money_returned"].toDouble()},
                         {"detail", QObject::tr("Sold Return : %1").arg(newData["barcode"].toString())}};
            mDb->insert("transactions", d);
        }
    } else if(newStatus == PURCHASE_RETURN_STATUS::RETURNED) {
        QVariantMap d{{"date", newData["return_date"]}, {"number", newData["barcode"]},
                     {"transaction_total", -newData["money_returned"].toDouble()}, {"machine_id", newData["machine_id"]},
                     {"bank_id", newData["bank_id"]}, {"money_total", -newData["money_returned"].toDouble()},
                     {"detail", QObject::tr("Sold Return : %1").arg(newData["barcode"].toString())}};
        mDb->where("link_type =", TRANSACTION_LINK_TYPE::SOLD_RETURN)->where("link_id =", newData["id"])->
                update("transactions", d);
        if(newRet != oldRet)
            addCount += newRet - oldRet;
    }
    if(addCount != 0) {
        ItemUtil util(mDb);
        util.updateStockCardCount(newData["barcode"].toString(), -(newCount - newRet), STOCK_CARD_TYPE::PURCHASE_RETURN, newData["id"].toInt(), true);
    }
}

void SoldReturnAction::afterDelete(const QVariantMap &oldData)
{
    ItemUtil util(mDb);
    util.updateStockCardCount(oldData["barcode"].toString(), 0, STOCK_CARD_TYPE::PURCHASE_RETURN, oldData["id"].toInt(), true);
    mDb->where("link_type =", TRANSACTION_LINK_TYPE::SOLD_RETURN)->where("link_id =", oldData["id"])->del("transactions");
}
