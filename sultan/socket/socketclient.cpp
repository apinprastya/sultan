/*
 * socketclient.cpp
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
#include "socketclient.h"
#include <QDebug>
#include <QTimer>
#include <QWebSocket>

#define TIMEOUT 30000  // in mili second
#define PING_TIME 5000 // in mili second
static QString TAG{"[SOCKETCLIENT]"};

SocketClient::SocketClient(QObject *parent)
    : QObject(parent),
      mSocket(new QWebSocket(QStringLiteral("Sultan Client"), QWebSocketProtocol::VersionLatest, this)) {
    connect(mSocket, SIGNAL(connected()), SIGNAL(socketConnected()));
    connect(mSocket, SIGNAL(connected()), SLOT(pong()));
    connect(mSocket, SIGNAL(pong(quint64, QByteArray)), SLOT(pong()));
    connect(mSocket, SIGNAL(disconnected()), SIGNAL(socketDisconnected()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(errorOccure()));
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(binaryMessageReceived(QByteArray)), SLOT(binaryMessageReceived(QByteArray)));
}

void SocketClient::connectToServer(const QString &address, int port) {
    mSocket->open(QUrl(QString("ws://%1:%2/").arg(address).arg(port)));
    QTimer::singleShot(TIMEOUT, this, SLOT(checkConnection()));
}

void SocketClient::disconnectFromServer() { mSocket->close(); }

QString SocketClient::lastError() { return mSocket->errorString(); }

bool SocketClient::isConnected() { return mSocket->state() == QAbstractSocket::ConnectedState; }

void SocketClient::sendMessage(LibG::Message *msg) { mSocket->sendBinaryMessage(msg->toByteArray()); }

void SocketClient::checkConnection() {
    if (mSocket->state() != QAbstractSocket::ConnectedState) {
        qCritical() << TAG << "Connection to server timeout";
        mSocket->close();
        emit connectionTimeout();
    }
}

void SocketClient::errorOccure() {
    qCritical() << TAG << "Connection error :" << mSocket->errorString();
    emit socketError();
}

void SocketClient::stateChanged(QAbstractSocket::SocketState state) { qDebug() << TAG << "State change :" << state; }

void SocketClient::binaryMessageReceived(const QByteArray &data) {
    LibG::Message msg(data);
    emit messageReceived(&msg);
}

void SocketClient::startPing() {
    if (mSocket->state() == QAbstractSocket::ConnectedState)
        mSocket->ping();
}

void SocketClient::pong() { QTimer::singleShot(PING_TIME, this, SLOT(startPing())); }
