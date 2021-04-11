/*
 * sockethandler.h
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
#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>

namespace LibG {
class Message;
}

class QWebSocket;

class SocketHandler : public QObject {
    Q_OBJECT
  public:
    SocketHandler(int id, QWebSocket *socket, QObject *parent = nullptr);
    void sendMessage(LibG::Message *msg);
    inline int getId() { return mId; }
    inline QWebSocket *getSocket() { return mSocket; }

  signals:
    void newMessage(LibG::Message *msg);
    void disconnect();

  private:
    int mId;
    QWebSocket *mSocket;

  private slots:
    void binaryMessageRecieved(const QByteArray &data);
};

#endif // SOCKETHANDLER_H
