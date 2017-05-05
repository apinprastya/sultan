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

#include "global_global.h"
#include <QVariantMap>

namespace LibG {

class GLOBALSHARED_EXPORT Message
{
private:
    /*
     * Flag are use to store type, status, and request/answer
     * type : 8bit
     * command : 8bit
     * status : 4bit    -> Code = 0: success; 1: error; ...
     * */
    int mFlag;
    /*
     * Store the websocketclient id if it message is for websocket
     * */
    int mSocketId;

    QVariantMap mData;

    int mUniqueId;

public:
    Message();
    Message(int type, int status = 0);
    Message(const QByteArray &ba);

    void setType(int type);
    void setCommand(int command);
    void setStatus(int status);
    inline int type() { return (mFlag & 0xFF); }
    inline int command() { return ((mFlag >> 8) & 0xFF); }
    inline int status() { return ((mFlag >> 16) & 0xF); }

    QJsonObject toJsonObject();
    QString toJsonString();
    QByteArray toByteArray();
    void fromByteArray(const QByteArray &ba);
    void fromJsonDoc(const QJsonDocument &doc);
};

}
#endif // MESSAGE_H
