/*
 * core.cpp
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

#include "core.h"
#include "global_constant.h"
#include "preference.h"
#include "global_setting_const.h"
#include "db.h"
#include "socket/socketmanager.h"
#include "socket/socketclient.h"
#include "mainserver.h"
#include "messagebus.h"
#include "migration.h"
#include "usersession.h"
#include "mainwindow.h"
#include "util.h"
#ifdef USE_LIBUSB
#include "usb.h"
#endif
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDate>
#include <functional>
#include <QDebug>

using namespace LibG;

static QString TAG{"CORE"};

Core::Core(QObject *parent) :
    QObject(parent),
    mSocketManager(nullptr),
    mSocketClient(new SocketClient(this)),
    mMainServer(nullptr),
    mMessageBus(new MessageBus(this)),
    mMainWindow(new LibGUI::MainWindow(mMessageBus))
{
    connect(mSocketClient, SIGNAL(socketConnected()), SLOT(clientConnected()));
    connect(mSocketClient, SIGNAL(socketDisconnected()), SLOT(clientDisconnected()));
    connect(mSocketClient, SIGNAL(connectionTimeout()), SLOT(connectionTimeout()));
    connect(mSocketClient, SIGNAL(messageReceived(LibG::Message*)), mMessageBus, SLOT(messageRecieved(LibG::Message*)));
    connect(mMessageBus, SIGNAL(newMessageToSend(LibG::Message*)), mSocketClient, SLOT(sendMessage(LibG::Message*)));
#ifdef Q_OS_LINUX
    qApp->setWindowIcon(QIcon(":/images/icon_64.png"));
#endif
    Util::init(qApp->applicationDirPath());
#ifdef USE_LIBUSB
    Usb::init();
#endif
}

Core::~Core()
{
    qDebug() << TAG << "Application Exited";
    Preference::destroy();
#ifdef USE_LIBUSB
    Usb::destroy();
#endif
    if(mMainWindow) delete mMainWindow;
    UserSession::destroy();
}

void Core::setup()
{
    mMainWindow->showSplashScreen();
    QTimer::singleShot(1000, this, SLOT(init()));
    mMainWindow->setSettingSocketOpenClose([=](const QString &address, int port) {
        if(mSettingSocketClient == nullptr) {
            mSettingSocketClient = new SocketClient(this);
            connect(mSettingSocketClient, SIGNAL(socketConnected()), SLOT(settingSocketConnected()));
            connect(mSettingSocketClient, SIGNAL(socketError()), SLOT(settingSocketError()));
            connect(mSettingSocketClient, SIGNAL(connectionTimeout()), SLOT(settingSocketTimeout()));
        }
        mSettingSocketClient->connectToServer(address, port);
    }, [=]() {
        if(mSettingSocketClient != nullptr) {
            if(mSettingSocketClient->isConnected()) mSettingSocketClient->disconnectFromServer();
            mSettingSocketClient->deleteLater();
            mSettingSocketClient = nullptr;
        }
    });
}

bool Core::migrationCallback(const QString &str)
{
    Q_UNUSED(str)
    return true;
}

void Core::init()
{
    qDebug() << TAG << "Initialize application";
    if(!LibG::Preference::getBool(SETTING::SETTING_OK, false)) {
        //the setting is not OK, so open the setting
        mMainWindow->hideSplashScreen();
        mMainWindow->showSetting();
    } else {
        if(Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
            qDebug() << "[CORE]" << "database type :" << Preference::getString(SETTING::DATABASE);
            qApp->processEvents();
            LibDB::Db::setDatabaseType(Preference::getString(SETTING::DATABASE));
            LibDB::Db::setDbSetting(
                    Preference::getString(SETTING::MYSQL_HOST),
                    Preference::getInt(SETTING::MYSQL_PORT),
                    Preference::getString(SETTING::MYSQL_USERNAME),
                    Preference::getString(SETTING::MYSQL_PASSWORD),
                    Preference::getString(SETTING::MYSQL_DB));
            QString error;
            if(!LibDB::Db::checkConnection(error)) {
                mMainWindow->showRestartError(tr("Database Error"), error);
                return;
            }
            mMainWindow->splashShowMessage("Migrate database ...");
            qApp->processEvents();
            const QString migrationpath = Preference::getString(SETTING::DATABASE) == "MYSQL" ?
                        "migration_mysql" : "migration_sqlite";
             LibDB::Migration::setAfterMigrate(std::bind(&Core::migrationCallback, this, std::placeholders::_1));
#ifdef Q_OS_MAC
            if(!LibDB::Migration::migrateAll(qApp->applicationDirPath() % "/../Resources/" % migrationpath, Preference::getString(SETTING::DATABASE))) {
#else
            if(!LibDB::Migration::migrateAll(qApp->applicationDirPath() % "/" + migrationpath, Preference::getString(SETTING::DATABASE))) {
#endif
                qCritical() << TAG << "Error migration";
                mMainWindow->splashShowMessage("Migrate database failed");
                qApp->processEvents();
                mMainWindow->showRestartError(tr("Database Error"), tr("Migrate database failed"));
                return;
            }
            mMainWindow->splashShowMessage("Start action server ...");
            qApp->processEvents();
            mMainServer = new LibServer::MainServer(this);
            mMainWindow->splashShowMessage("Start socket server ...");
            qApp->processEvents();
            mSocketManager = new SocketManager(this);
            if(!mSocketManager->listen(Preference::getInt(SETTING::APP_PORT))) {
                mMainWindow->showRestartError(tr("Server Socket Error"), tr("Port already in used.\nPossible another Sultan already openned"));
                return;
            }
            connect(mSocketManager, SIGNAL(receivedMessage(LibG::Message*)), mMainServer, SLOT(messageReceived(LibG::Message*)));
            connect(mMainServer, SIGNAL(messageReady(LibG::Message*)), mSocketManager, SLOT(sendToClient(LibG::Message*)));
            mMainWindow->splashShowMessage("Connecting to server ...");
            qApp->processEvents();
            QTimer::singleShot(10, this, SLOT(connectToServer()));
        } else {
            mMainWindow->splashShowMessage("Connecting to server ...");
            qApp->processEvents();
            QTimer::singleShot(10, this, SLOT(connectToServer()));
        }
    }
}

void Core::connectToServer()
{
    if(Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
        mSocketClient->connectToServer(QStringLiteral("localhost"), Preference::getInt(SETTING::APP_PORT));
    } else {
        mSocketClient->connectToServer(Preference::getString(SETTING::SERVER_ADDRESS),
                                       Preference::getInt(SETTING::SERVER_PORT));
    }
}

void Core::clientConnected()
{
    mMainWindow->hideSplashScreen();
    mMainWindow->showMainWindow();
}

void Core::clientDisconnected()
{
    mMainWindow->showRestartError(tr("Error Disconnect"), tr("Connection to server lost. Please check your connectivity."));
}

void Core::connectionTimeout()
{
    mMainWindow->showRestartError(tr("Error Timeout"), tr("Connection to server timeout. Please check your connectivity."));
}

void Core::settingSocketConnected()
{
    mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_SUCCESS, "");
}

void Core::settingSocketError()
{
    mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_FAILED, mSettingSocketClient->lastError());
}

void Core::settingSocketTimeout()
{
    mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_TIMEOUT, "");
}
