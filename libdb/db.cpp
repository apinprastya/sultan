/*
 * db.cpp
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

#include "db.h"
#include "easylogging++.h"
#include <QUuid>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlDriver>
#include <QStringBuilder>

using namespace LibDB;

static std::string TAG = "DB";
QStringList Db::mCreated = QStringList();
QStringList Db::mUpdate = QStringList();
QStringList Db::mSoftDelete = QStringList();
#ifdef QT_DEBUG
bool Db::DEBUG = true;
#else
bool Db::DEBUG = false;
#endif

Db::Db():
    parent(NULL),
    mConnectionName(QUuid::createUuid().toString())
{
    mDebug = DEBUG;
}

Db::~Db()
{
    if(parent == NULL) {
        {
            QSqlDatabase db = QSqlDatabase::database(mConnectionName);
            if(db.isOpen())
                db.close();
        }
        QSqlDatabase::removeDatabase(mConnectionName);
        for(int i = 0; i < childs.size(); i++)
            delete childs[i];
    }
}

Db *Db::createInstance(const QString &host, int port, const QString &username, const QString &password, const QString &dbname)
{
    Db* db = new Db();
    db->init(host, port, username, password, dbname);
    return db;
}

Db *Db::reset()
{
    mWhere = QString();
    mSelect = QString();
    mJoin = QString();
    mTable = QString();
    mLimit = QString();
    mStart = QString();
    mSort = QString();
    mGroup = QString();
    return this;
}

Db *Db::select(const QString &value)
{
    if(!mSelect.isEmpty())
        mSelect.append(QLatin1String(","));
    mSelect.append(value);
    return this;
}

Db *Db::table(const QString &value)
{
    mTable = value;
    return this;
}

Db *Db::where(const QString &value, const QVariant &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QLatin1String(" AND "));
    if(!val.isValid())
        mWhere.append(value);
    else {
        mWhere.append(value);
        if(val.type() == QVariant::String)
            mWhere.append(QLatin1String("'") + val.toString() + QLatin1String("'"));
        else
            mWhere.append(QString::number(val.toInt()));
    }
    return this;
}

Db *Db::like(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QLatin1String(" AND "));
    mWhere.append(value);
    if(val.startsWith(QLatin1String("%")) || val.endsWith(QLatin1String("%"))) {
        mWhere.append(QLatin1String(" LIKE '"));
        mWhere.append(val);
        mWhere.append(QLatin1String("'"));
    } else {
        mWhere.append(QLatin1String(" LIKE '%"));
        mWhere.append(val);
        mWhere.append(QLatin1String("%'"));
    }
    return this;
}

Db *Db::likePost(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QLatin1String(" AND "));
    mWhere.append(value);
    mWhere.append(QLatin1String(" LIKE '"));
    mWhere.append(val);
    mWhere.append(QLatin1String("%'"));
    return this;
}

Db *Db::likeNative(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QLatin1String(" AND "));
    mWhere.append(value);
    mWhere.append(QLatin1String(" LIKE '"));
    mWhere.append(val);
    mWhere.append(QLatin1String("'"));
    return this;
}

Db *Db::whereOr(const QString &value)
{
    if (!mWhere.isEmpty())
        mWhere.append(QLatin1String(" OR "));
    mWhere.append(value);
    return this;
}

Db *Db::join(const QString &value)
{
    if(!mJoin.isEmpty())
        mJoin.append(QLatin1String(" "));
    mJoin.append(value);
    return this;
}

Db *Db::limit(int limit)
{
    mLimit = QString::number(limit);
    return this;
}

Db *Db::start(int start)
{
    mStart = QString::number(start);
    return this;
}

Db *Db::sort(const QString &sort)
{
    if(!mSort.isEmpty())
        mSort.append(QLatin1String(", "));
    mSort.append(sort);
    return this;
}

Db *Db::group(const QString &group)
{
    mGroup = group;
    return this;
}

QString Db::getSelectQuery(const QString &select)
{
    QString sql = QString(QLatin1String("SELECT "));
    /*if(mSoftDelete.contains(mTable))
        where(QLatin1String("deleted = 0"));*/
    if(mSelect.isEmpty())
        mSelect.append(QLatin1String("*"));
    if(!select.isEmpty())
        sql.append(select);
    else
        sql.append(mSelect);
    sql.append(QLatin1String(" FROM ")).append(mTable).append(QLatin1String(" "));
    if(!mJoin.isEmpty())
        sql.append(mJoin);
    if(!mWhere.isEmpty())
        sql.append(QLatin1String(" WHERE ")).append(mWhere);
    if(!mGroup.isEmpty())
        sql.append(QLatin1String(" GROUP BY ")).append(mGroup);
    if(!mSort.isEmpty())
        sql.append(QLatin1String(" ORDER BY ")).append(mSort);
    if(!mLimit.isEmpty())
        sql.append(QLatin1String(" LIMIT ")).append(mLimit);
    if(!mStart.isEmpty())
        sql.append(QLatin1String(" OFFSET ")).append(mStart);
    if(mDebug)
        qDebug() << sql;
    return sql;
}

QSqlError Db::lastError() { return mLastError; }

QVariant Db::lastInsertedId() { return mInsertedId; }

QString Db::lastQuery() { return mLastQuery; }

DbResult Db::exec()
{
    QList<QVariant> result;
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    if (query.exec(getSelectQuery())) {
        QSqlRecord record = query.record();
        while(query.next()) {
            QVariantMap map;
            for(int i = 0; i < record.count(); i++)
                map.insert(record.fieldName(i), query.value(i));
            result.append(map);
        }
    }
    reset();
    postQuery(&query);
    return DbResult(result);
}

bool Db::exec(const QString &sqlcommand)
{
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    bool ok = query.exec(sqlcommand);
    reset();
    postQuery(&query);
    if(mDebug)
        qDebug() << sqlcommand;
    if(!ok) {
        LOG(ERROR) << TAG << lastQuery();
        LOG(ERROR) << TAG << mLastError.text();
    }
    return ok;
}

bool Db::insert(const QString &table, const QVariantMap &data)
{
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    QString sql = QString(QLatin1String("INSERT INTO "));
    QString values = QString(QLatin1String("VALUES ("));
    sql.append(table).append(QLatin1String(" ("));
    const QList<QString> &keys = data.keys();
    for(int i = 0; i < keys.size(); i++) {
        sql.append(keys.at(i));
        values.append(QLatin1String("?"));
        if(i != keys.size() - 1) {
            sql.append(QLatin1String(","));
            values.append(QLatin1String(","));
        }
    }
    sql.append(QLatin1String(") ")).append(values).append(QLatin1String(")"));
    query.prepare(sql);
    for(int i = 0; i < keys.size(); i++)
        query.bindValue(i, data[keys.at(i)]);
    reset();
    bool res = query.exec();
    LOG(INFO) << TAG << query.lastQuery() << dataToString(data);
    postQuery(&query);
    if(!res) {
        LOG(ERROR) << TAG << mLastError.text();
    }
    return res;
}

bool Db::update(const QString &table, const QVariantMap &data)
{
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    QString sql = QString(QLatin1String("UPDATE "));
    sql.append(table).append(QLatin1String(" SET "));
    if(mUpdate.contains(table)) {
        sql.append(QLatin1String("updated_at = NOW() "));
    }
    const QList<QString> &keys = data.keys();
    for(int i = 0; i < keys.size(); i++) {
        sql.append(keys.at(i));
        if(!data.value(keys.at(i)).isNull()) {
            sql.append(QLatin1String(" = ?"));
        }
        if(i != keys.size() - 1)
            sql.append(QLatin1String(","));
    }
    sql.append(QLatin1String(" WHERE ")).append(mWhere);
    query.prepare(sql);
    for(int i = 0; i < keys.size(); i++)
        query.bindValue(i, data[keys.at(i)]);
    reset();
    bool res = query.exec();
    LOG(INFO) << TAG << query.lastQuery() << dataToString(data);
    postQuery(&query);
    if(!res) {
        LOG(ERROR) << TAG << mLastError.text();
    }
    return res;
}

bool Db::del(const QString &table)
{
    if(mSoftDelete.contains(table)) {
        QVariantMap map;
        map.insert(QLatin1String("deleted"), 1);
        return update(table, map);
    } else {
        QSqlQuery query(QSqlDatabase::database(mConnectionName));
        QString sql = QString(QLatin1String("DELETE FROM "));
        sql.append(table).append(QLatin1String(" WHERE ")).append(mWhere);
        reset();
        bool res = query.exec(sql);
        LOG(INFO) << TAG << query.lastQuery();
        postQuery(&query);
        if(!res) {
            LOG(ERROR) << TAG << mLastError.text();
        }
        return res;
    }
}

int Db::count()
{
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    Db *db = clone();
    QString sql = db->getSelectQuery(QLatin1String("count(*)"));
    query.exec(sql);
    postQuery(&query);
    if(query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

Db *Db::clone()
{
    Db *db = new Db();
    *db = *this;
    db->parent = this;
    this->childs.push_back(db);
    return db;
}

bool Db::beginTransaction()
{
    return QSqlDatabase::database(mConnectionName).transaction();
}

bool Db::commit()
{
    return QSqlDatabase::database(mConnectionName).commit();
}

bool Db::roolback()
{
    return QSqlDatabase::database(mConnectionName).rollback();
}

void Db::init(const QString &host, int port, const QString &username, const QString &password, const QString &dbname)
{
    auto database = QSqlDatabase::addDatabase(QLatin1String("QMYSQL"), mConnectionName);
    mSupportTransaction = database.driver()->hasFeature(QSqlDriver::Transactions);
    database.setPort(port);
    database.setDatabaseName(dbname);
    database.setHostName(host);
    database.setUserName(username);
    database.setPassword(password);
    database.open();
    reset();
}

void Db::postQuery(QSqlQuery *query)
{
    mLastError = query->lastError();
    mLastQuery = query->lastQuery();
    mInsertedId = query->lastInsertId();
}

QString Db::dataToString(const QVariantMap &map)
{
    QString str;
    str.append(QLatin1String("{"));
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        str.append(i.key()).append(QLatin1String(": ")).append(i.value().toString()).append(QLatin1String(","));
    }
    str.append(QLatin1String("}"));
    return str;
}

