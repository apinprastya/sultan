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

using namespace LibServer;

SellPriceActions::SellPriceActions():
    ServerAction("sellprices", "id")
{
    mFlag = HAS_UPDATE_FIELD;
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
