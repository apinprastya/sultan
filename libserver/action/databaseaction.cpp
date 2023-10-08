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
#include "db.h"
#include "dbutil.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "migration.h"
#include "preference.h"
#include "queryhelper.h"
#include "util.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QStringRef>
#include <QTemporaryFile>
#include <QTextStream>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

DatabaseAction::DatabaseAction() : ServerAction("", "") {
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&DatabaseAction::exportDatabase, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::IMPORT, std::bind(&DatabaseAction::importDatabase, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::RESET, std::bind(&DatabaseAction::resetDatabase, this, std::placeholders::_1));
}

LibG::Message DatabaseAction::exportDatabase(LibG::Message *msg) {
    Message message(msg);
    auto dbtype = Preference::getString(SETTING::DATABASE);
    QStringList tableList;
    DbResult migRes = mDb->get("migrations");
    const QString &version = getVersionMigration(migRes.first()["name"].toString());
    if (dbtype == "SQLITE") {
        DbResult res = mDb->where("type = ", "table")->get("sqlite_master");
        for (int i = 0; i < res.size(); i++)
            tableList << res.data(i)["tbl_name"].toString();
    } else {
        DbResult res = mDb->execResult("SHOW TABLES");
        for (int i = 0; i < res.size(); i++)
            tableList << res.data(i).first().toString();
    }

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    QByteArray buffer;
    QTextStream stream(&buffer);
    stream << version << "\n";
    for (int i = 0; i < tableList.size(); i++) {
        int offset = 0;
        int limit = 500;
        stream << "TABLE|" << tableList[i] << "|";
        DbResult res = mDb->start(offset)->limit(limit)->get(tableList[i]);
        if (res.isEmpty())
            stream << "EMPTY\n";
        while (!res.isEmpty()) {
            if (offset == 0) {
                const QVariantMap &m = res.first();
                const QList<QString> &k = m.keys();
                for (auto _k : k) {
                    stream << _k << ";";
                }
                stream << "\n";
            }
            for (int j = 0; j < res.size(); j++) {
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
    const QByteArray &arr = qCompress(buffer);
    message.addData("data", QString(arr.toBase64()));
    return message;
}

LibG::Message DatabaseAction::importDatabase(LibG::Message *msg) {
    Message message(msg);
    const QString b64 = msg->data("data").toString();
    const QByteArray &ba = QByteArray::fromBase64(b64.toUtf8());
    const QByteArray &uncompressed = qUncompress(ba);
    importData(uncompressed, &message);
    return message;
}

Message DatabaseAction::resetDatabase(Message *msg) {
    Message message(msg);
    DbResult res = mDb->where("id = ", msg->data("user_id"))->get("users");
    if (res.size() != 1) {
        message.setError(QObject::tr("User id not found"));
    } else {
        const QVariantMap &data = res.first();
        const QString &pass = data["password"].toString();
        if (pass.compare(msg->data("password").toString()) != 0) {
            message.setError("Password is wrong!");
        } else {
            auto dbtype = Preference::getString(SETTING::DATABASE);
            if (dbtype == "SQLITE") {
                QDir dir = QDir::home();
                QString dirpath = Preference::getString(SETTING::SQLITE_DBPATH);
                QString dbname = Preference::getString(SETTING::SQLITE_DBNAME);
                if (dbname.isEmpty())
                    dbname = "sultan.db";
                if (!dbname.endsWith(".db"))
                    dbname += ".db";
                if (dirpath.isEmpty()) {
#ifdef Q_OS_WIN32
                    dir.cd(".sultan");
#else
                    dir.cd(".sultan");
#endif
                }
                // TODO: check this when this happen on client side
                Preference::setValue(SETTING::RESETDB, true);
            } else {
                mDb->exec("DROP DATABASE " + Preference::getString(SETTING::MYSQL_DB));
            }
        }
    }
    return message;
}

void DatabaseAction::importData(const QByteArray &ba, LibG::Message * /*msg*/) {
    auto dbtype = Preference::getString(SETTING::DATABASE);
    QString ver = "013";
    int counter = -1;
    QTextStream stream(ba);
    QStringList columns;
    QString tableName;
    bool versionOK = false;
    while (!stream.atEnd()) {
        const QString &line = stream.readLine();
        if (!versionOK) {
            const QString &strVersion = line.trimmed();
            if (strVersion.size() == 3) {
                ver = strVersion;
                migrateUntil(ver);
                versionOK = true;
                continue;
            }
        }
        if (line.startsWith("TABLE|")) {
            columns.clear();
            const QVector<QStringRef> &lsplit = line.splitRef("|", QString::SkipEmptyParts);
            if (counter >= 0) {
                counter = -1;
                mDb->commit();
            }
            if (lsplit.size() == 3) {
                tableName = lsplit[1].toString();
                if (dbtype == "SQLITE") {
                    mDb->exec(QString("DELETE FROM %1").arg(tableName));
                } else {
                    mDb->exec(QString("TRUNCATE TABLE %1").arg(tableName));
                }
                if (!lsplit[2].compare(QLatin1String("EMPTY")))
                    continue;
                const QVector<QStringRef> &colsplit = lsplit[2].split(";", QString::SkipEmptyParts);
                for (auto col : colsplit)
                    columns.append(col.toString());
            }
        } else {
            const QVector<QStringRef> &lsplit = line.splitRef(";");
            QVariantMap d;
            for (int i = 0; i < columns.size(); i++) {
                if (!lsplit[i].isEmpty())
                    d.insert(columns[i], lsplit[i].toString().replace("#$", ";").replace("#%", "\n"));
            }
            if (counter < 0)
                mDb->beginTransaction();
            if (!mDb->insert(tableName, d))
                qWarning() << "[IMPORT] " << mDb->lastError().text();
            counter++;
            if (counter >= 500) {
                mDb->commit();
                counter = -1;
            }
        }
    }
    if (counter >= 0)
        mDb->commit();
}

void DatabaseAction::migrateUntil(const QString &version) {
    auto dbtype = Preference::getString(SETTING::DATABASE);
    if (dbtype == "SQLITE") {
        QDir dir = QDir::home();
        QString dirpath = Preference::getString(SETTING::SQLITE_DBPATH);
        QString dbname = Preference::getString(SETTING::SQLITE_DBNAME);
        if (dbname.isEmpty())
            dbname = "sultan.db";
        if (!dbname.endsWith(".db"))
            dbname += ".db";
        if (dirpath.isEmpty()) {
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
    auto func = [version, this](const QString &name) -> bool {
        const QString &ver = getVersionMigration(name);
        if (!ver.compare(version))
            return false;
        return true;
    };
    QDirIterator it(
        QString(":/migration/%1").arg(Preference::getString(SETTING::DATABASE) == "MYSQL" ? "mysql" : "sqlite"),
        QDirIterator::Subdirectories);
    QStringList sl;
    while (it.hasNext()) {
        sl.append(it.next());
    }
    sl.sort();
    LibDB::Migration::migrateAll(sl, Preference::getString(SETTING::DATABASE), func);
}

QString DatabaseAction::getVersionMigration(const QString &fullpath) {
    if (fullpath.contains("/")) {
        auto list = fullpath.split("/");
        qDebug() << list;
        qDebug() << list.last();
        return list.last().left(3);
    }
    return fullpath;
}
