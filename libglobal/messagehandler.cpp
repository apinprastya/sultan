/*
 * messagehandler.cpp
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
#include "messagehandler.h"
#include "message.h"
#include "messagebus.h"

using namespace LibG;

static int UNIQUE_ID = 0;

MessageHandler::MessageHandler():
    mMessageBus(nullptr)
{

}

MessageHandler::~MessageHandler()
{
    if(mMessageBus != nullptr)
        mMessageBus->removeHandler(this);
}

void MessageHandler::setMessageBus(MessageBus *bus)
{
    mMessageBus = bus;
    bus->registerHandler(this);
}

bool MessageHandler::consumeMessage(Message *msg)
{
    if(mAlwaysListen.contains(msg->type())) {
        messageReceived(msg);
    } else if(mInterests.contains(msg->getUniqueId())) {
        mInterests.removeOne(msg->getUniqueId());
        messageReceived(msg);
        return true;
    }
    return false;
}

int MessageHandler::sendMessage(Message *msg)
{
    int unique = UNIQUE_ID++;
    msg->setUniqueId(unique);
    mInterests.append(unique);
    mMessageBus->sendMessage(msg);
    return msg->getUniqueId();
}

void MessageHandler::setAlwaysListen(int msg_type)
{
    if(!mAlwaysListen.contains(msg_type)) {
        mAlwaysListen.append(msg_type);
    }
}

void MessageHandler::removeAlwaysListern(int msg_type)
{
    mAlwaysListen.removeOne(msg_type);
}
