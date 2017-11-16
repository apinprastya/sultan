/*
 * itemlinkaction.cpp
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
#include "itemlinkaction.h"
#include "db.h"

using namespace LibServer;
using namespace LibDB;

ItemLinkAction::ItemLinkAction():
    ServerAction("itemlinks", "id")
{
    mFlag = USE_TRANSACTION;
}

void ItemLinkAction::selectAndJoin()
{
    mDb->select("itemlinks.*, (select name from items where barcode = itemlinks.barcode) as name, \
                (select name from items where barcode = itemlinks.barcode_link) as name_link, \
                (select buy_price from items where barcode = itemlinks.barcode_link) as buy_price, \
                (select price from sellprices where barcode = itemlinks.barcode_link order by count asc limit 1) as sell_price");
}
