/*
 * core.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "preference.h"
#include "global_setting_const.h"
#include "easylogging++.h"
#include "db.h"
#include <QApplication>
#include <QDebug>

using namespace LibG;

static std::string TAG = "CORE";

Core::Core(QObject *parent) : QObject(parent)
{
    connect(qApp, SIGNAL(aboutToQuit()), SLOT(appExited()));
}

void Core::init()
{
    LOG(INFO) << TAG << "Initialize application";
    Preference::createInstance();
    if(LibG::Preference::getBool(SETTING::SETTING_OK, false)) {
        /*LibDB::Db::setDbSetting(
                    Preference::getString(SETTING::MYSQL_HOST),
                    Preference::getInt(SETTING::MYSQL_PORT),
                    Preference::getString(SETTING::MYSQL_USERNAME),
                    Preference::getString(SETTING::MYSQL_PASSWORD),
                    Preference::getString(SETTING::MYSQL_DB));*/
    } else {
        //the setting is not OK, so open the setting
    }
    qDebug() << LibG::Preference::getBool(SETTING::SETTING_OK, false);
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

void Core::appExited()
{
    LOG(INFO) << TAG << "Application Exited";
    Preference::destroy();
}
