/*
 * router.cpp
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
#include "router.h"
#include "message.h"
#include "serveraction.h"
#include "global_constant.h"
#include "action/useraction.h"
#include "action/suplieraction.h"

using namespace LibServer;
using namespace LibG;

Router::Router()
{
}

LibG::Message Router::handler(LibG::Message msg)
{
    auto action = getServerAction(msg.type());
    if(action != nullptr)
        return action->exec(&msg);
    msg.setStatus(STATUS::ERROR);
    return msg;
}

ServerAction *Router::getServerAction(int type)
{
    switch(type) {
    case MSG_TYPE::USER:
        return new UserAction();
    case MSG_TYPE::SUPLIER:
        return new SuplierAction();
    }
    return nullptr;
}

