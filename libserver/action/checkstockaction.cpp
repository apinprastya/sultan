/*
 *
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

#include "checkstockaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "global_constant.h"

using namespace LibServer;

CheckStockAction::CheckStockAction():
    ServerAction("checkstocks", "id")
{
    mFlag = AFTER_INSERT;
}

void CheckStockAction::afterInsert(const QVariantMap &data)
{
    mDb->where("barcode = ", data["barcode"]);
    QVariantMap d{{"stock", data["real_stock"]}};
    mDb->update("items", d);
}
