/*
 * sockethandler.cpp
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
#include "sockethandler.h"
#include "message.h"
#include <QTcpSocket>

SocketHandler::SocketHandler(int id, WrapTcpSocket *socket, QObject *parent)
    : QObject(parent), mId(id), mSocket(socket) {
    connect(mSocket, SIGNAL(disconnected()), SIGNAL(disconnect()));
    connect(mSocket, SIGNAL(binaryMessageReceived(QByteArray)), SLOT(binaryMessageRecieved(QByteArray)));
}

void SocketHandler::sendMessage(LibG::Message *msg) { mSocket->sendBinaryMessage(msg->toByteArray()); }

void SocketHandler::binaryMessageRecieved(const QByteArray &data) {
    LibG::Message msg(data);
    msg.setSocketId(mId);
    emit newMessage(&msg);
}
