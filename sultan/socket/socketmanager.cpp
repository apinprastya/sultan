/*
 * socketmanager.cpp
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
#include "socketmanager.h"
#include "global_constant.h"
#include "message.h"
#include "sockethandler.h"
#include <QDebug>
#include <QTcpServer>

static QString TAG{"[SOCKETMANAGER]"};

SocketManager::SocketManager(QObject *parent) : QObject(parent), mLastId(0), mServer(new QTcpServer(this)) {
    connect(mServer, SIGNAL(newConnection()), SLOT(newConnection()));
}

bool SocketManager::listen(int port) {
    bool ret = mServer->listen(QHostAddress::Any, port);
    if (!ret)
        qCritical() << TAG << "Unable to start socket server :" << mServer->errorString();
    else
        qDebug() << TAG << "Server listening on port :" << port;
    return ret;
}

void SocketManager::newConnection() {
    while (mServer->hasPendingConnections()) {
        auto socket = mServer->nextPendingConnection();
        auto handler = new SocketHandler(mLastId++, new WrapTcpSocket(socket), this);
        qDebug() << TAG << "New socket connection" << socket->peerAddress().toString() << socket->peerPort();
        mHandlers.insert(handler->getId(), handler);
        connect(handler, SIGNAL(disconnect()), SLOT(clientDisconnect()));
        connect(handler, SIGNAL(newMessage(LibG::Message *)), SLOT(processMessageFromClient(LibG::Message *)));
    }
}

void SocketManager::clientDisconnect() {
    auto handler = static_cast<SocketHandler *>(QObject::sender());
    mHandlers.remove(handler->getId());
    qDebug() << TAG << "Client disconnect" << handler->getSocket()->peerAddress().toString();
}

void SocketManager::sendToClient(LibG::Message *msg) {
    if (mHandlers.contains(msg->getSocketId()))
        mHandlers[msg->getSocketId()]->sendMessage(msg);
}

void SocketManager::processMessageFromClient(LibG::Message *msg) {
    if (msg->isType(LibG::MSG_TYPE::BROADCAST)) {
        QMap<int, SocketHandler *>::const_iterator i = mHandlers.constBegin();
        while (i != mHandlers.constEnd()) {
            i.value()->sendMessage(msg);
            ++i;
        }
    } else {
        emit receivedMessage(msg);
    }
}
