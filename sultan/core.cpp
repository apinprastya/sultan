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
#include "customerdisplaymanager.h"
#include "db.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "mainserver.h"
#include "messagebus.h"
#include "migration.h"
#include "preference.h"
#include "socket/socketclient.h"
#include "socket/socketmanager.h"
#include "usersession.h"
#ifndef SERVER_BUILD
#include "mainwindow.h"
#else
#include "dummy/guidummy.h"
#endif
#include "util.h"
#ifdef USE_LIBUSB
#include "usb.h"
#endif
#include "http/httpserver.h"
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStringBuilder>
#include <QTimer>
#include <functional>

using namespace LibG;

static QString TAG{"CORE"};

Core::Core(QObject *parent)
    : QObject(parent), mSocketManager(nullptr), mSocketClient(new SocketClient(this)), mMainServer(nullptr),
      mMessageBus(new MessageBus(this)),
#ifdef SERVER_BUILD
      mMainWindow(new GuiDummy())
#else
      mMainWindow(new LibGUI::MainWindow(mMessageBus))
#endif
{
    connect(mSocketClient, SIGNAL(socketConnected()), SLOT(clientConnected()));
    connect(mSocketClient, SIGNAL(socketDisconnected()), SLOT(clientDisconnected()));
    connect(mSocketClient, SIGNAL(connectionTimeout()), SLOT(connectionTimeout()));
    connect(mSocketClient, SIGNAL(messageReceived(LibG::Message *)), mMessageBus,
            SLOT(messageRecieved(LibG::Message *)));
    connect(mMessageBus, SIGNAL(newMessageToSend(LibG::Message *)), mSocketClient, SLOT(sendMessage(LibG::Message *)));
#ifdef Q_OS_LINUX
#ifndef SERVER_BUILD
    qApp->setWindowIcon(QIcon(":/images/icon_64.png"));
#endif
#endif
    Util::init(qApp->applicationDirPath());
#ifdef USE_LIBUSB
    Usb::init();
#endif
}

Core::~Core() {
    qDebug() << TAG << "Application Exited";
    Preference::destroy();
#ifdef USE_LIBUSB
    Usb::destroy();
#endif
    if (mMainWindow)
        delete mMainWindow;
    UserSession::destroy();
}

void Core::setup() {
    mMainWindow->showSplashScreen();
#ifdef SERVER_BUILD
    QTimer::singleShot(1, this, SLOT(init()));
#else
    QTimer::singleShot(1000, this, SLOT(init()));
#endif
    mMainWindow->setSettingSocketOpenClose(
        [=](const QString &address, int port) {
            if (mSettingSocketClient == nullptr) {
                mSettingSocketClient = new SocketClient(this);
                connect(mSettingSocketClient, SIGNAL(socketConnected()), SLOT(settingSocketConnected()));
                connect(mSettingSocketClient, SIGNAL(socketError()), SLOT(settingSocketError()));
                connect(mSettingSocketClient, SIGNAL(connectionTimeout()), SLOT(settingSocketTimeout()));
            }
            mSettingSocketClient->connectToServer(address, port);
        },
        [=]() {
            if (mSettingSocketClient != nullptr) {
                if (mSettingSocketClient->isConnected())
                    mSettingSocketClient->disconnectFromServer();
                mSettingSocketClient->deleteLater();
                mSettingSocketClient = nullptr;
            }
        });
}

bool Core::initConfigJson() {
    QDir dir(qApp->applicationDirPath());
    QFile file(dir.absoluteFilePath("setting.json"));
    if (!file.exists()) {
        qCritical() << "setting.json can not be found at " << dir.absoluteFilePath("setting.json");
        qApp->exit(1);
        return false;
    }
    if (!file.open(QFile::ReadOnly)) {
        qCritical() << "Can not open setting file " << dir.absoluteFilePath("setting.json");
        qApp->exit(2);
        return false;
    }
    QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
    Preference::setValue(SETTING::DATABASE, json.value("database").toString("SQLITE"));
    Preference::setValue(SETTING::MYSQL_HOST, json.value("mysql_host").toString());
    Preference::setValue(SETTING::MYSQL_USERNAME, json.value("mysql_username").toString());
    Preference::setValue(SETTING::MYSQL_PASSWORD, json.value("mysql_password").toString());
    Preference::setValue(SETTING::MYSQL_PORT, json.value("mysql_port").toInt());
    Preference::setValue(SETTING::MYSQL_DB, json.value("mysql_dbname").toString());
    Preference::setValue(SETTING::SQLITE_DBPATH, json.value("sqlite_db_path").toString());
    Preference::setValue(SETTING::SQLITE_DBNAME, json.value("sqlite_db_name").toString());
    Preference::setValue(SETTING::APP_PORT, json.value("app_port").toInt(4545));
    Preference::setValue(SETTING::APP_TYPE, APPLICATION_TYPE::SERVER);
    return true;
}

bool Core::migrationCallback(const QString &str) {
    Q_UNUSED(str)
    return true;
}

void Core::init() {
#ifdef SERVER_BUILD
    if (!initConfigJson())
        return;
#endif
    qDebug() << TAG << "Initialize application";
    LibPrint::CustomerDisplayManager::createInstance(this);
    if (!LibG::Preference::getBool(SETTING::SETTING_OK, false)) {
        // the setting is not OK, so open the setting
        mMainWindow->hideSplashScreen();
        mMainWindow->showSetting();
    } else {
        if (Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
            qDebug() << "[CORE]"
                     << "database type :" << Preference::getString(SETTING::DATABASE);
            qApp->processEvents();
            LibDB::Db::setDatabaseType(Preference::getString(SETTING::DATABASE));
            LibDB::Db::setDbSetting(Preference::getString(SETTING::MYSQL_HOST), Preference::getInt(SETTING::MYSQL_PORT),
                                    Preference::getString(SETTING::MYSQL_USERNAME),
                                    Preference::getString(SETTING::MYSQL_PASSWORD),
                                    Preference::getString(SETTING::MYSQL_DB));
            QString error;
            if (!LibDB::Db::checkConnection(error)) {
                mMainWindow->showRestartError(tr("Database Error"), error);
                return;
            }
            mMainWindow->splashShowMessage("Migrate database ...");
            qApp->processEvents();
            QDirIterator it(
                QString(":/migration/%1").arg(Preference::getString(SETTING::DATABASE) == "MYSQL" ? "mysql" : "sqlite"),
                QDirIterator::Subdirectories);
            QStringList sl;
            while (it.hasNext()) {
                sl.append(it.next());
            }
            sl.sort();
            LibDB::Migration::setAfterMigrate(std::bind(&Core::migrationCallback, this, std::placeholders::_1));
            if (!LibDB::Migration::migrateAll(sl, Preference::getString(SETTING::DATABASE))) {
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
            if (!mSocketManager->listen(Preference::getInt(SETTING::APP_PORT))) {
                mMainWindow->showRestartError(tr("Server Socket Error"),
                                              tr("Port already in used.\nPossible another Sultan already openned"));
#ifdef SERVER_BUILD
                qApp->exit(100);
#endif
                return;
            }
            connect(mSocketManager, SIGNAL(receivedMessage(LibG::Message *)), mMainServer,
                    SLOT(messageReceived(LibG::Message *)));
            connect(mMainServer, SIGNAL(messageReady(LibG::Message *)), mSocketManager,
                    SLOT(sendToClient(LibG::Message *)));
            mHttpServer = new HttpServer();
            mHttpServer->runServer(Preference::getInt(SETTING::APP_PORT) + 1);
            mMainWindow->splashShowMessage("Connecting to server ...");
            qApp->processEvents();
#ifndef SERVER_BUILD
            QTimer::singleShot(10, this, SLOT(connectToServer()));
#endif
        } else {
            mMainWindow->splashShowMessage("Connecting to server ...");
            qApp->processEvents();
            QTimer::singleShot(10, this, SLOT(connectToServer()));
        }
    }
}

void Core::connectToServer() {
    if (Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
        mSocketClient->connectToServer(QStringLiteral("localhost"), Preference::getInt(SETTING::APP_PORT));
    } else {
        mSocketClient->connectToServer(Preference::getString(SETTING::SERVER_ADDRESS),
                                       Preference::getInt(SETTING::SERVER_PORT));
    }
}

void Core::clientConnected() {
    mMainWindow->hideSplashScreen();
    mMainWindow->showMainWindow();
}

void Core::clientDisconnected() {
    mMainWindow->showRestartError(tr("Error Disconnect"),
                                  tr("Connection to server lost. Please check your connectivity."));
}

void Core::connectionTimeout() {
    mMainWindow->showRestartError(tr("Error Timeout"),
                                  tr("Connection to server timeout. Please check your connectivity."));
}

void Core::settingSocketConnected() { mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_SUCCESS, ""); }

void Core::settingSocketError() {
    mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_FAILED, mSettingSocketClient->lastError());
}

void Core::settingSocketTimeout() { mMainWindow->guiMessage(GUI_MESSAGE::MSG_CONNECTION_TIMEOUT, ""); }
