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

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ServerAction::ServerAction():
    mDb(Db::createInstance())
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
}

LibG::Message ServerAction::insert(LibG::Message *msg)
{
    LibG::Message message;
    return message;
}

LibG::Message ServerAction::update(LibG::Message *msg)
{

}

LibG::Message ServerAction::del(LibG::Message *msg)
{

}

Message ServerAction::get(Message *msg)
{

}

LibG::Message ServerAction::query(LibG::Message *msg)
{

}
