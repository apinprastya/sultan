/*
 * transactionaction.cpp
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
#include "transactionaction.h"
#include "global_constant.h"
#include "message.h"
#include "db.h"
#include "queryhelper.h"
#include "dbresult.h"
#include "preference.h"
#include <QStringBuilder>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

TransactionAction::TransactionAction():
    ServerAction("transactions", "id")
{
    mFunctionMap.insert(MSG_COMMAND::SUMMARY_TRANSACTION, std::bind(&TransactionAction::summaryTransaction, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::SUMMARY_MONEY, std::bind(&TransactionAction::summaryMoney, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&TransactionAction::exportData, this, std::placeholders::_1));
}

LibG::Message TransactionAction::summaryTransaction(LibG::Message *msg)
{
    Message message(msg);
    mDb->table(mTableName);
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    DbResult res = mDb->clone()->select("sum(transaction_total) as total")->exec();
    message.addData("total", res.first()["total"].toDouble());
    res = mDb->clone()->select("sum(transaction_total) as income")->where("type = ", TRANSACTION_TYPE::INCOME)->exec();
    message.addData("income", res.first()["income"].toDouble());
    res = mDb->clone()->select("sum(transaction_total) as expense")->where("type = ", TRANSACTION_TYPE::EXPENSE)->exec();
    message.addData("expense", res.first()["expense"].toDouble());
    return message;
}

LibG::Message TransactionAction::summaryMoney(LibG::Message *msg)
{
    Message message(msg);
    mDb->table(mTableName);
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    DbResult res = mDb->clone()->select("sum(money_total) as total")->exec();
    message.addData("total", res.first()["total"].toDouble());
    return message;
}

Message TransactionAction::exportData(Message *msg)
{
    LibG::Message message(msg);
    QString arr;
    arr.append("date;number;type;detail;total\n");
    mDb->table(mTableName);
    selectAndJoin();
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    mDb = QueryHelper::sort(mDb, msg->data());
    const int limit = 500;
    int start = 0;
    while(true) {
        DbResult res = mDb->clone()->start(start)->limit(limit)->exec();
        if(res.isEmpty()) break;
        for(int i = 0; i < res.size(); i++) {
            const QVariantMap &d = res.data(i);
            arr.append(d["date"].toString() % ";");
            arr.append(d["number"].toString() % ";");
            arr.append((d["type"].toInt() == TRANSACTION_TYPE::INCOME ? QObject::tr("Income") : QObject::tr("Expense")) % ";");
            arr.append(d["detail"].toString() % ";");
            arr.append(Preference::toString(d["transaction_total"].toDouble()) % ";\n");
        }
        start += limit;
    }
    message.addData("data", arr);
    return message;
}

QMap<QString, QString> TransactionAction::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("user", "users.username");
    map.insert("machine", "machines.name");
    map.insert("bank", "transactions.bank_id");
    return map;
}

void TransactionAction::selectAndJoin()
{
    mDb->table(mTableName)->select("transactions.*, users.username as user, banks.name as bank, machines.name as machine")->
            join("LEFT JOIN users ON transactions.user_id = users.id")->
            join("LEFT JOIN banks ON transactions.bank_id = banks.id")->
            join("LEFT JOIN machines ON transactions.machine_id = machines.id");
}
