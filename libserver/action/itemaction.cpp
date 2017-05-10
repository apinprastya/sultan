/*
 * itemaction.cpp
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
#include "itemaction.h"
#include "global_constant.h"
#include "db.h"
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ItemAction::ItemAction():
    ServerAction("items")
{
    mFunctionMap.insert(MSG_COMMAND::CASHIER_PRICE, std::bind(&ItemAction::prices, this, std::placeholders::_1));
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
