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
#include "easylogging++.h"
#include "db.h"
#include "gui/splash.h"
#include "gui/settingdialog.h"
#include "gui/logindialog.h"
#include "socket/socketmanager.h"
#include "socket/socketclient.h"
#include "mainserver.h"
#include "messagebus.h"
#include "migration.h"
#include "usersession.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDebug>

using namespace LibG;

static std::string TAG = "CORE";

Core::Core(QObject *parent) :
    QObject(parent),
    mSplashUi(new Splash()),
    mSettingDialog(new SettingDialog()),
    mLoginDialog(new LoginDialog()),
    mSocketManager(nullptr),
    mSocketClient(new SocketClient(this)),
    mMainServer(nullptr),
    mMessageBus(new MessageBus(this)),
    mMainWindow(new LibGUI::MainWindow(mMessageBus))
{
    Preference::createInstance();
    mLoginDialog->setMessageBus(mMessageBus);
    connect(mSocketClient, SIGNAL(socketConnected()), SLOT(clientConnected()));
    connect(mSocketClient, SIGNAL(socketDisconnected()), SLOT(clientDisconnected()));
    connect(mSocketClient, SIGNAL(messageReceived(LibG::Message*)), mMessageBus, SLOT(messageRecieved(LibG::Message*)));
    connect(mMessageBus, SIGNAL(newMessageToSend(LibG::Message*)), mSocketClient, SLOT(sendMessage(LibG::Message*)));
    connect(mLoginDialog, SIGNAL(loginSuccess()), SLOT(loginSuccess()));
    connect(mMainWindow, SIGNAL(logout()), SLOT(logout()));
#ifdef Q_OS_LINUX
    qApp->setWindowIcon(QIcon(":/images/icon_64.png"));
#endif
}

Core::~Core()
{
    LOG(INFO) << TAG << "Application Exited";
    Preference::destroy();
    if(mSplashUi) delete mSplashUi;
    if(mSettingDialog) delete mSettingDialog;
    if(mLoginDialog) delete mLoginDialog;
    if(mMainWindow) delete mMainWindow;
    UserSession::destroy();
}

void Core::setup()
{
    mSplashUi->show();
    QTimer::singleShot(1000, this, SLOT(init()));
}

void Core::initLogger()
{
    auto appDir = QDir(qApp->applicationDirPath());
    el::Configurations conf(appDir.absoluteFilePath(QLatin1String("log.conf")).toStdString());
    if(!appDir.cd(QLatin1String("logs"))) {
        appDir.mkdir(appDir.absolutePath() + QLatin1String("/logs"));
        appDir.cd(QLatin1String("logs"));
    }
    conf.setGlobally(el::ConfigurationType::Filename, appDir.absoluteFilePath(QLatin1String("log.log")).toStdString());
    el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::reconfigureAllLoggers(conf);
    el::Helpers::installPreRollOutCallback([](const char* name, std::size_t /*size*/){
        const QString &newName = QLatin1String(name) + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMdd-hhmmss"));
        QFile::rename(QLatin1String(name), newName);
        QProcess::startDetached("/bin/gzip", QStringList() << newName);
    });
}

void Core::init()
{
    initLogger();
    LOG(INFO) << TAG << "Initialize application";
    if(!LibG::Preference::getBool(SETTING::SETTING_OK, false)) {
        //the setting is not OK, so open the setting
        mSplashUi->hide();
        mSettingDialog->showDialog();
        //show setting ui
    } else {
        if(Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
            mSplashUi->setMessage("Setting Mysql connection ...");
            qApp->processEvents();
            LibDB::Db::setDbSetting(
                    Preference::getString(SETTING::MYSQL_HOST),
                    Preference::getInt(SETTING::MYSQL_PORT),
                    Preference::getString(SETTING::MYSQL_USERNAME),
                    Preference::getString(SETTING::MYSQL_PASSWORD),
                    Preference::getString(SETTING::MYSQL_DB));
            QString error;
            if(!LibDB::Db::checkConnection(error)) {
                return;
            }
            mSplashUi->setMessage("Migrate database ...");
            qApp->processEvents();
            if(!LibDB::Migration::migrateAll(qApp->applicationDirPath() % "/migrations")) {
                LOG(ERROR) << TAG << "Error migration";
                mSplashUi->setMessage("Migrate database failed");
                qApp->processEvents();
                return;
            }
            mSplashUi->setMessage("Start action server ...");
            qApp->processEvents();
            mMainServer = new LibServer::MainServer(this);
            mSplashUi->setMessage("Start socket server ...");
            qApp->processEvents();
            mSocketManager = new SocketManager(this);
            if(!mSocketManager->listen(Preference::getInt(SETTING::APP_PORT))) {
                return;
            }
            connect(mSocketManager, SIGNAL(receivedMessage(LibG::Message*)), mMainServer, SLOT(messageReceived(LibG::Message*)));
            connect(mMainServer, SIGNAL(messageReady(LibG::Message*)), mSocketManager, SLOT(sendToClient(LibG::Message*)));
            mSplashUi->setMessage("Connecting to server ...");
            qApp->processEvents();
            QTimer::singleShot(10, this, SLOT(connectToServer()));
        } else {
            mSplashUi->setMessage("Connecting to server ...");
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
    mSplashUi->hide();
    mLoginDialog->showDialog();
}

void Core::clientDisconnected()
{

}

void Core::loginSuccess()
{
    mMainWindow->show();
}

void Core::logout()
{
    mMainWindow->hide();
    mLoginDialog->showDialog();
}
