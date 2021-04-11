/*
 * cusomerdebtaction.cpp
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
#include "cusomercreditaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include <QDateTime>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

CusomerCreditAction::CusomerCreditAction() : ServerAction("customercredits", "id") {
    mFlag = AFTER_INSERT | USE_TRANSACTION;
}

void CusomerCreditAction::afterInsert(const QVariantMap &data) {
    auto cust_id = data["customer_id"].toInt();
    mDb->exec(QString("UPDATE customers SET credit = (SELECT SUM(credit) FROM customercredits WHERE customer_id = %1) "
                      "WHERE id = %2")
                  .arg(cust_id)
                  .arg(cust_id));
    QVariantMap d{{"date", QDateTime::currentDateTime()},
                  {"number", data["number"]},
                  {"type", TRANSACTION_TYPE::INCOME},
                  {"link_id", data["id"]},
                  {"link_type", TRANSACTION_LINK_TYPE::CUSTOMER_CREDIT},
                  {"transaction_total", 0},
                  {"user_id", data["user_id"]},
                  {"machine_id", data["machine_id"]},
                  {"bank_id", data["bank_id"]},
                  {"money_total", -data["credit"].toDouble()},
                  {"detail", QObject::tr("Customer credit : %1").arg(data["number"].toString())}};
    mDb->insert("transactions", d);
}
