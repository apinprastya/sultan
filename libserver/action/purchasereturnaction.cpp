/*
 * purchasereturnaction.cpp
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
#include "purchasereturnaction.h"
#include "global_constant.h"
#include "db.h"
#include "queryhelper.h"
#include "util.h"
#include "dbresult.h"
#include <QStringBuilder>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

PurchaseReturnAction::PurchaseReturnAction():
    ServerAction("purchaseitemreturns", "id")
{
    mFlag = HAS_UPDATE_FIELD | AFTER_INSERT | AFTER_UPDATE;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&PurchaseReturnAction::summary, this, std::placeholders::_1));
}

LibG::Message PurchaseReturnAction::summary(LibG::Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->select("sum(total) as total")->where("status = ", PURCHASE_RETURN_STATUS::UNRETURN)->
            get(mTableName);
    message.setData(res.first());
    return message;
}

void PurchaseReturnAction::selectAndJoin()
{
    mDb->select(mTableName % ".*, users.name as user, supliers.name as suplier, banks.name as bank")->
            join("LEFT JOIN users ON users.id = purchaseitemreturns.user_id")->
            join("LEFT JOIN supliers ON supliers.id = purchaseitemreturns.suplier_id")->
            join("LEFT JOIN banks ON banks.id = purchaseitemreturns.bank_id");
}

QMap<QString, QString> PurchaseReturnAction::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("user", "users.name");
    map.insert("suplier", "supliers.name");
    map.insert("name", "purchaseitemreturns.name");
    return map;
}

void PurchaseReturnAction::afterInsert(const QVariantMap &data)
{
    mDb->exec(QString("UPDATE items SET stock = stock - '%1' WHERE barcode = '%2'").arg(data["count"].toFloat()).arg(data["barcode"].toString()));
}

void PurchaseReturnAction::afterUpdate(const QVariantMap &oldData, const QVariantMap &newData)
{
    QString query{"UPDATE items SET stock = stock + '%1' WHERE barcode = '%2'"};
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
        } else {
            addCount += newRet;
        }
    } else if(newRet != oldRet && newStatus == PURCHASE_RETURN_STATUS::RETURNED) {
        addCount += newRet - oldRet;
    }
    mDb->exec(query.arg(addCount).arg(newData["barcode"].toString()));
}
