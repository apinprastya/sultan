/*
 * cusomerdebtaction.cpp
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
#include "cusomercreditaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"

using namespace LibServer;
using namespace LibDB;

CusomerCreditAction::CusomerCreditAction():
    ServerAction("customercredits", "id")
{
    mFlag = AFTER_INSERT;
}

void CusomerCreditAction::afterInsert(const QVariantMap &data)
{
    auto cust_id = data["customer_id"].toInt();
    mDb->exec(QString("UPDATE customers SET credit = (SELECT SUM(credit) FROM customercredits WHERE customer_id = %1) WHERE id = %2").
              arg(cust_id).arg(cust_id));
}
