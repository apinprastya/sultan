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
#include "sockethandler.h"
#include "message.h"
#include "global_constant.h"
#include <QWebSocket>
#include <QWebSocketServer>
#include <QDebug>

static QString TAG{"[SOCKETMANAGER]"};

SocketManager::SocketManager(QObject *parent):
    QObject(parent),
    mLastId(0),
    mServer(new QWebSocketServer(QStringLiteral("Sultan"), QWebSocketServer::NonSecureMode, this))
{
    connect(mServer, SIGNAL(newConnection()), SLOT(newConnection()));
}

bool SocketManager::listen(int port)
{
    bool ret = mServer->listen(QHostAddress::Any, port);
    if(!ret)
        qCritical() << TAG << "Unable to start socket server :" << mServer->errorString();
    else
        qDebug() << TAG << "Server listening on port :" << port;
    return ret;
}

void SocketManager::newConnection()
{
    while(mServer->hasPendingConnections()) {
        auto socket = mServer->nextPendingConnection();
        auto handler = new SocketHandler(mLastId++, socket, this);
        qDebug() << TAG << "New socket connection" << socket->peerAddress().toString() << socket->peerPort();
        mHandlers.insert(handler->getId(), handler);
        connect(handler, SIGNAL(disconnect()), SLOT(clientDisconnect()));
        connect(handler, SIGNAL(newMessage(LibG::Message*)), SLOT(processMessageFromClient(LibG::Message*)));
    }
}

void SocketManager::clientDisconnect()
{
    auto handler = static_cast<SocketHandler*>(QObject::sender());
    mHandlers.remove(handler->getId());
    qDebug() << TAG << "Client disconnect" << handler->getSocket()->peerAddress().toString();
}

void SocketManager::sendToClient(LibG::Message *msg)
{
    if(mHandlers.contains(msg->getSocketId()))
        mHandlers[msg->getSocketId()]->sendMessage(msg);
}

void SocketManager::processMessageFromClient(LibG::Message *msg)
{
    if(msg->isType(LibG::MSG_TYPE::BROADCAST)) {
        for(int i = 0; i < mHandlers.size(); i++)
            mHandlers[i]->sendMessage(msg);
    } else {
        emit receivedMessage(msg);
    }
}
