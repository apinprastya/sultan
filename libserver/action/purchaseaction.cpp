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
#include "db.h"

using namespace LibServer;
using namespace LibDB;

PurchaseAction::PurchaseAction():
    ServerAction("purchases", "id")
{
    mFlag = HAS_UPDATE_FIELD;
    //TODO: transaction
    //TODO: money
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
