/*
 * socketclient.h
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
#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "message.h"
#include <QAbstractSocket>
#include <QObject>

class QWebSocket;

class SocketClient : public QObject {
    Q_OBJECT
  public:
    SocketClient(QObject *parent = nullptr);
    void connectToServer(const QString &address, int port);
    void disconnectFromServer();
    QString lastError();
    bool isConnected();

  public slots:
    void sendMessage(LibG::Message *msg);

  private:
    QWebSocket *mSocket;

  signals:
    void socketConnected();
    void socketError();
    void socketDisconnected();
    void messageReceived(LibG::Message *msg);
    void connectionTimeout();

  private slots:
    void checkConnection();
    void errorOccure();
    void stateChanged(QAbstractSocket::SocketState state);
    void binaryMessageReceived(const QByteArray &data);
    void startPing();
    void pong();
};

#endif // SOCKETCLIENT_H
