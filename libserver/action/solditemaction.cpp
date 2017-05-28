/*
 * solditemaction.cpp
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
#include "solditemaction.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

SoldItemAction::SoldItemAction():
    ServerAction("solditems", "id")
{
    mFunctionMap.insert(MSG_COMMAND::SOLD_SUMMARY, std::bind(&SoldItemAction::getSummary, this, std::placeholders::_1));
}

void SoldItemAction::selectAndJoin()
{
    mDb->select("*, (total - buy_price) as margin");
}

Message SoldItemAction::getSummary(Message *msg)
{
    LibG::Message message(msg);
    mDb->table(mTableName);
    mDb->select("sum(total) as total, sum(total - buy_price) as margin");
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    message.setData(mDb->exec().first());
    return message;
}
