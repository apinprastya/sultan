/*
 * wrapsocket.cpp
 * Copyright 2023, Apin <apin.klas@gmail.com>
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

#include "wraptcpsocket.h"
#include <QDataStream>
#include <QHostAddress>
#include <QTimer>

#define PING_TIME 5000 // in mili second

#define HEADER 1988

#define PING_MESSAGE 1
#define NORMAL_MESSAGE 2

WrapTcpSocket::WrapTcpSocket(QTcpSocket *socket) : QObject(socket), mSocket(socket) {
    connect(socket, SIGNAL(connected()), SIGNAL(connected()));
    connect(socket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SIGNAL(error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SIGNAL(stateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void WrapTcpSocket::connectToHost(const QString &host, int port) { mSocket->connectToHost(host, port); }

void WrapTcpSocket::close() { mSocket->close(); }

QString WrapTcpSocket::errorString() const { return mSocket->errorString(); }

QAbstractSocket::SocketState WrapTcpSocket::state() const { return mSocket->state(); }

void WrapTcpSocket::sendBinaryMessage(const QByteArray &message) {
    QDataStream stream(mSocket);
    stream << HEADER;
    stream << NORMAL_MESSAGE;
    stream << message.length();
    stream.writeRawData(message.data(), message.length());
}

void WrapTcpSocket::ping() {
    QDataStream stream(mSocket);
    stream << HEADER;
    stream << PING_MESSAGE;
}

QHostAddress WrapTcpSocket::peerAddress() const { return mSocket->peerAddress(); }

void WrapTcpSocket::readMessage(QIODevice *device) {
    int left = mTotalLength - mTotalRead;
    int read = device->read(mData.data() + mTotalRead, left);
    mTotalRead += read;
    if (mTotalRead == mTotalLength) {
        mTotalLength = -1;
        emit binaryMessageReceived(mData);
    }
}

void WrapTcpSocket::readyRead() {
    while (mSocket->bytesAvailable()) {
        if (mTotalLength < 0) {
            QDataStream stream(mSocket);
            int header;
            stream >> header;
            if (header != HEADER)
                continue;
            int type;
            stream >> type;
            if (type == PING_MESSAGE) {
                QTimer::singleShot(PING_TIME, this, SLOT(ping()));
                continue;
            }
            if (type == NORMAL_MESSAGE) {
                int length;
                stream >> length;
                mData.resize(length);
                mTotalLength = length;
                mTotalRead = 0;
                readMessage(mSocket);
            }
        } else {
            readMessage(mSocket);
        }
    }
}
