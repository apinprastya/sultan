/*
 * core.h
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

#ifndef CORE_H
#define CORE_H

#include <QObject>

class SocketManager;
class SocketClient;
class HttpServer;

namespace LibG {
class MessageBus;
class AbstractSultanGUI;
}

namespace LibServer {
class MainServer;
}

namespace LibGUI {
class MainWindow;
}

class Core : public QObject
{
    Q_OBJECT
public:
    Core(QObject *parent = nullptr);
    ~Core();
    void setup();

private:
    SocketManager *mSocketManager;
    SocketClient *mSocketClient;
    HttpServer *mHttpServer;
    LibServer::MainServer *mMainServer;
    LibG::MessageBus *mMessageBus;
    LibG::AbstractSultanGUI *mMainWindow;
    SocketClient *mSettingSocketClient = nullptr;

protected:
    bool initConfigJson();
    bool migrationCallback(const QString &str);

private slots:
    void init();
    void connectToServer();
    void clientConnected();
    void clientDisconnected();
    void connectionTimeout();
    void settingSocketConnected();
    void settingSocketError();
    void settingSocketTimeout();
};

#endif // CORE_H
