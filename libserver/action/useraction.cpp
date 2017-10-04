/*
 * useraction.cpp
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
#include "useraction.h"
#include "global_constant.h"
#include "db.h"
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

UserAction::UserAction():
    ServerAction("users", "id")
{
    mFlag = HAS_UPDATE_FIELD | USE_TRANSACTION | SOFT_DELETE;
    mFunctionMap.insert(MSG_COMMAND::LOGIN, std::bind(&UserAction::login, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::CHANGE_MY_PASSWORD, std::bind(&UserAction::changeMyPassword, this, std::placeholders::_1));
}

Message UserAction::login(Message *msg)
{
    LibG::Message message(msg);
    const QString &username = msg->data("username").toString();
    const QString &password = msg->data("password").toString();
    mDb->table(mTableName)->where("username = ", username);
    DbResult res = mDb->exec();
    if(res.isEmpty()) {
        message.setError("Username and password not match");
    } else {
        const QVariantMap &data = res.first();
        if(password.compare(data["password"].toString())) {
            message.setError("Username and password not match");
        } else {
            message.addData("id", data["id"]);
            message.addData("username", data["username"]);
            message.addData("name", data["name"]);
            message.addData("permission", data["permission"]);
        }
    }
    return message;
}

Message UserAction::changeMyPassword(Message *msg)
{
    LibG::Message message(msg);
    const QString &old = msg->data("current").toString();
    const QString &newpass = msg->data("new").toString();
    mDb->table(mTableName)->where("id = ", msg->data("id").toInt());
    DbResult res = mDb->exec();
    if(res.isEmpty()) {
        message.setError("User not found");
    } else {
        const QVariantMap &data = res.first();
        if(old.compare(data["password"].toString())) {
            message.setError("Old password wrong");
        } else {
            QVariantMap d{{"password", newpass}};
            mDb->beginTransaction();
            mDb->where("id = ", msg->data("id").toInt())->update(mTableName, d);
            if(!mDb->commit()) {
                message.setError(mDb->lastError().text());
            }
        }
    }
    return message;
}
