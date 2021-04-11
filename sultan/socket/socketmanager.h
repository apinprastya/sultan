/*
 * socketmanager.h
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
#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QMap>
#include <QObject>

namespace LibG {
class Message;
}

class QWebSocketServer;
class SocketHandler;

class SocketManager : public QObject {
    Q_OBJECT
  public:
    SocketManager(QObject *parent = nullptr);
    bool listen(int port);

  private:
    int mLastId;
    QWebSocketServer *mServer;
    QMap<int, SocketHandler *> mHandlers;

  private slots:
    void newConnection();
    void clientDisconnect();

  public slots:
    void sendToClient(LibG::Message *msg);
    void processMessageFromClient(LibG::Message *msg);

  signals:
    void receivedMessage(LibG::Message *msg);
};

#endif // SOCKETMANAGER_H
