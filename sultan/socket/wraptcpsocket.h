/*
 * wrapsocket.h
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

#ifndef WRAPTCPSOCKET_H
#define WRAPTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>

class WrapTcpSocket : public QObject
{
    Q_OBJECT
  public:
    WrapTcpSocket(QTcpSocket *socket);
    void connectToHost(const QString &host, int port);
    void close();
    QString errorString() const;
    QAbstractSocket::SocketState state() const;
    void sendBinaryMessage(const QByteArray &message);
    QHostAddress peerAddress() const;

  private:
    QTcpSocket *mSocket;
    int mTotalLength = -1;
    int mTotalRead = 0;
    QByteArray mData;

    void readMessage(QIODevice *device);

  private slots:
    void readyRead();

  signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void stateChanged(QAbstractSocket::SocketState);
    void binaryMessageReceived(const QByteArray &);

  public slots:
    void ping();
};

#endif // WRAPTCPSOCKET_H
