/*
 * databaseaction.cpp
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
#include "databaseaction.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "db.h"
#include "queryhelper.h"
#include "util.h"
#include "preference.h"
#include "dbutil.h"
#include "migration.h"
#include <QStringRef>
#include <QStringBuilder>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

DatabaseAction::DatabaseAction():
    ServerAction("", "")
{
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&DatabaseAction::exportDatabase, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::IMPORT, std::bind(&DatabaseAction::importDatabase, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::RESET, std::bind(&DatabaseAction::resetDatabase, this, std::placeholders::_1));
}

LibG::Message DatabaseAction::exportDatabase(LibG::Message *msg)
{
    Message message(msg);
    message.addData("data", exportData().toBase64());
    return message;
}

LibG::Message DatabaseAction::importDatabase(LibG::Message *msg)
{
    Message message(msg);
    QTemporaryFile f;
    {
        const QByteArray &d = qUncompress(QByteArray::fromBase64(msg->data("data").toString().toUtf8()));
        f.open();
        f.write(d);
        f.close();
    }
    importData(f.fileName(), msg->data("version").toString(), &message);
    return message;
}

Message DatabaseAction::resetDatabase(Message *msg)
{
    Message message(msg);
    DbResult res = mDb->where("id = ", msg->data("user_id"))->get("users");
    if(res.size() != 1) {
        message.setError(QObject::tr("User id not found"));
    } else {
        const QVariantMap &data = res.first();
        const QString &pass = data["password"].toString();
        if(pass.compare(msg->data("password").toString()) != 0) {
            message.setError("Password is wrong!");
        } else {
            auto dbtype = Preference::getString(SETTING::DATABASE);
            if(dbtype == "SQLITE") {
                QDir dir = QDir::home();
                QString dirpath = Preference::getString(SETTING::SQLITE_DBPATH);
                QString dbname = Preference::getString(SETTING::SQLITE_DBNAME);
                if(dbname.isEmpty()) dbname = "sultan.db";
                if(!dbname.endsWith(".db")) dbname += ".db";
                if(dirpath.isEmpty()) {
#ifdef Q_OS_WIN32
                    dir.cd("sultan");
#else
                    dir.cd(".sultan");
#endif
                }
                QFile::remove(dir.absoluteFilePath(dbname));
            } else {
                mDb->exec("DROP DATABASE " + Preference::getString(SETTING::MYSQL_DB));
            }
        }
    }
    return message;
}

QByteArray DatabaseAction::exportData()
{
    auto dbtype = Preference::getString(SETTING::DATABASE);
    QStringList tableList;
    DbResult migRes = mDb->get("migrations");
    const QString &version = migRes.first()["name"].toString().left(3);
    if(dbtype == "SQLITE") {
        DbResult res = mDb->where("type = ", "table")->get("sqlite_master");
        for(int i = 0; i < res.size(); i++)
            tableList << res.data(i)["tbl_name"].toString();
    } else {
        DbResult res = mDb->execResult("SHOW TABLES");
        for(int i = 0; i < res.size(); i++)
            tableList << res.data(i).first().toString();
    }
    QTemporaryFile file;
    file.open();
    QTextStream stream(&file);
    stream << version << "\n";
    for(int i = 0; i < tableList.size(); i++) {
        int offset = 0;
        int limit = 500;
        stream << "TABLE|" << tableList[i] << "|";
        DbResult res = mDb->start(offset)->limit(limit)->get(tableList[i]);
        if(res.isEmpty()) stream << "EMPTY\n";
        while(!res.isEmpty()) {
            if(offset == 0) {
                const QVariantMap &m = res.first();
                const QList<QString> &k = m.keys();
                for(auto _k : k) {
                    stream << _k << ";";
                }
                stream << "\n";
            }
            for(int j = 0; j < res.size(); j++) {
                const QVariantMap &d = res.data(j);
                QMapIterator<QString, QVariant> it(d);
                while (it.hasNext()) {
                    it.next();
                    stream << it.value().toString().replace(";", "#$").replace("\n", "#%") << ";";
                }
                stream << "\n";
            }
            offset += limit;
            res = mDb->start(offset)->limit(limit)->get(tableList[i]);
        }
    }
    stream.flush();
    file.reset();
    const QByteArray &arr = file.readAll();
    file.close();
    return qCompress(arr);
}

void DatabaseAction::importData(const QString &fileName, const QString &version, LibG::Message *msg)
{
    auto dbtype = Preference::getString(SETTING::DATABASE);
    QString ver = "013";
    QFile f(fileName);
    if(f.open(QFile::ReadOnly)) {
        QTextStream stream(&f);
        QStringList columns;
        QString tableName;
        bool versionOK = false;
        while(!stream.atEnd()) {
            const QString &line = stream.readLine();
            if(!versionOK) {
                const QString &strVersion = line.trimmed();
                if(strVersion.size() == 3) ver = strVersion;
                migrateUntil(ver);
                versionOK = true;
            }
            if(line.startsWith("TABLE|")) {
                columns.clear();
                const QVector<QStringRef> &lsplit = line.splitRef("|", QString::SkipEmptyParts);
                if(lsplit.size() == 3) {
                    tableName = lsplit[1].toString();
                    if(dbtype == "SQLITE") {
                        mDb->exec(QString("DELETE FROM %1").arg(tableName));
                    } else {
                        mDb->exec(QString("TRUNCATE TABLE %1").arg(tableName));
                    }
                    if(!lsplit[2].compare(QLatin1String("EMPTY"))) continue;
                    const QVector<QStringRef> &colsplit = lsplit[2].split(";", QString::SkipEmptyParts);
                    for(auto col : colsplit)
                        columns.append(col.toString());
                }
            } else {
                const QVector<QStringRef> &lsplit = line.splitRef(";");
                QVariantMap d;
                for(int i = 0; i < columns.size(); i++) {
                    if(!lsplit[i].isEmpty())
                        d.insert(columns[i], lsplit[i].toString().replace("#$", ";").replace("#%", "\n"));
                }
                if(!mDb->insert(tableName, d))
                    qWarning() << "[IMPORT] " << mDb->lastError().text();
            }
        }
        f.close();
    }
}

void DatabaseAction::migrateUntil(const QString &version)
{
    auto dbtype = Preference::getString(SETTING::DATABASE);
    if(dbtype == "SQLITE") {
        QDir dir = QDir::home();
        QString dirpath = Preference::getString(SETTING::SQLITE_DBPATH);
        QString dbname = Preference::getString(SETTING::SQLITE_DBNAME);
        if(dbname.isEmpty()) dbname = "sultan.db";
        if(!dbname.endsWith(".db")) dbname += ".db";
        if(dirpath.isEmpty()) {
#ifdef Q_OS_WIN32
            dir.cd("sultan");
#else
            dir.cd(".sultan");
#endif
        }
        QFile::remove(dir.absoluteFilePath(dbname));
    } else {
        mDb->exec("DROP DATABASE " + Preference::getString(SETTING::MYSQL_DB));
        mDb->exec("CREATE DATABASE " + Preference::getString(SETTING::MYSQL_DB));
    }
    const QString migrationpath = Preference::getString(SETTING::DATABASE) == "MYSQL" ?
                "migration_mysql" : "migration_sqlite";
    auto func = [version](const QString &name) -> bool {
        const QString &ver = name.left(3);
        if(!ver.compare(version)) return false;
        return true;
    };
#ifdef Q_OS_MAC
    LibDB::Migration::migrateAll(Util::appDir() % "/../Resources/" % migrationpath, Preference::getString(SETTING::DATABASE), func);
#else
    LibDB::Migration::migrateAll(Util::appDir() % "/" + migrationpath, Preference::getString(SETTING::DATABASE), func);
#endif
}

