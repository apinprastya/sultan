/*
 * message.h
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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVariantMap>

class Message
{
private:
    /*
     * Flag are use to store type, status, and request/answer
     * type : 16bit
     * status : 4bit    -> Code = 0: success; 1: error; ...
     * */
    int mFlag;
    /*
     * To store the type when answer the message
     * */
    int mFlagReply;
    /*
     * Store the websocketclient id if it message is for websocket
     * */
    int mSocketId;

    QVariantMap mData;

public:
    Message(int type, int status = 0);
    inline int type() { return (mFlag & 0xFFFF); }
    inline int status() { return ((mFlag >> 16) & 0xF); }
};

#endif // MESSAGE_H
