/*
 * customeraction.cpp
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
#include "customeraction.h"
#include "db.h"
#include "dbresult.h"
#include "global_constant.h"
#include "message.h"
#include "preference.h"
#include "queryhelper.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

CustomerAction::CustomerAction() : ServerAction("customers", "id") {
    mFlag = HAS_UPDATE_FIELD | USE_TRANSACTION | SOFT_DELETE | BEFORE_DELETE;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&CustomerAction::summary, this, std::placeholders::_1));
}

LibG::Message CustomerAction::summary(LibG::Message *msg) {
    Message message(msg);
    mDb->table(mTableName);
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    DbResult res = mDb->clone()->select("sum(credit) as credit, sum(reward) as reward")->exec();
    message.setData(res.first());
    return message;
}

bool CustomerAction::beforeDelete(const QVariantMap &oldData, Message *retMsg) {
    if (oldData["credit"].toDouble() != 0) {
        retMsg->setError(QObject::tr("Customer credit must 0 when delete the customer"));
        return false;
    }
    return true;
}
