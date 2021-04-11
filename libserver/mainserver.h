/*
 * libserver.h
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
#ifndef LIBSERVER_H
#define LIBSERVER_H

#include "server_global.h"
#include <QObject>

namespace LibG {
class Message;
}

namespace LibServer {

class Router;

class SERVERSHARED_EXPORT MainServer : public QObject {
    Q_OBJECT
  public:
    MainServer(QObject *parent = nullptr);
    ~MainServer();

  public slots:
    void messageReceived(LibG::Message *msg);

  signals:
    void messageReady(LibG::Message *msg);

  private:
    Router *mRouter;
};

} // namespace LibServer
#endif // LIBSERVER_H
