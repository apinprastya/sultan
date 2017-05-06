/*
 * serveraction.cpp
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
#include "serveraction.h"
#include "global_constant.h"
#include "db.h"
#include "queryhelper.h"
#include "easylogging++.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

static std::string TAG = "[SERVERACTION]";

ServerAction::ServerAction(const QString &tableName):
    mDb(Db::createInstance()),
    mTableName(tableName)
{
    mFunctionMap.insert(MSG_COMMAND::INSERT, std::bind(&ServerAction::insert, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::UPDATE, std::bind(&ServerAction::update, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::DELETE, std::bind(&ServerAction::del, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::GET, std::bind(&ServerAction::del, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::QUERY, std::bind(&ServerAction::get, this, std::placeholders::_1));
}

ServerAction::~ServerAction()
{
    delete mDb;
}

Message ServerAction::exec(Message *msg)
{
    if(mFunctionMap.contains(msg->command()))
        return mFunctionMap[msg->command()](msg);
    LOG(ERROR) << TAG << "Command not available : " << msg->command() << " with type : " << msg->type();
    Message message = *msg;
    message.setError("Command not available");
    return message;
}

LibG::Message ServerAction::insert(LibG::Message *msg)
{
    LibG::Message message(msg);
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res = mDb->where("id = ", mDb->lastInsertedId())->get(mTableName);
        message.setData(res.first());
    }
    return message;
}

LibG::Message ServerAction::update(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->where("id = ", msg->data("id"));
    if(!mDb->update(mTableName, msg->data("data").toMap())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res = mDb->where("id = ", msg->data("id"))->get(mTableName);
        message.setData(res.first());
    }
    return message;
}

LibG::Message ServerAction::del(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->where("id = ", msg->data("id"));
    if(!mDb->del(mTableName)) {
        message.setError(mDb->lastError().text());
    }
    return message;
}

Message ServerAction::get(Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->where("id = ", mDb->lastInsertedId())->get(mTableName);
    if(res.isEmpty()) {
        message.setError("Data not found");
    } else {
        message.setData(res.first());
    }
    return message;
}

LibG::Message ServerAction::query(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    if(!(msg->data().contains(QStringLiteral("start")) && msg->data().value(QStringLiteral("start")).toInt() > 0))
        message.addData(QStringLiteral("total"), mDb->count());
    setStart(&message, msg);
    mDb = QueryHelper::limitOffset(mDb, msg->data());
    mDb = QueryHelper::sort(mDb, msg->data());
    message.addData(QStringLiteral("data"), mDb->exec().data());
    return message;
}

void ServerAction::setStart(Message *msg, Message *src)
{
    if(src->data().contains(QStringLiteral("start"))) {
        msg->addData(QStringLiteral("start"), src->data(QStringLiteral("start")).toInt());
    }
}

QMap<QString, QString> ServerAction::fieldMap() const
{
    return QMap<QString, QString>();
}
