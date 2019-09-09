/*
 * db.cpp
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

#include "db.h"
#include "preference.h"
#include "global_setting_const.h"
#include <QUuid>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QThreadStorage>
#include <QDir>

using namespace LibDB;
using namespace LibG;

static QString TAG{"[DB]"};
static DBSetting DBSETTING;
static QString DBTYPE = "MYSQL";
static QMap<QString, QString> SQLDRIVERNAME{{"MYSQL", "QMYSQL"}, {"SQLITE", "QSQLITE"}};
QStringList Db::mCreated = QStringList();
QStringList Db::mUpdate = QStringList();
QStringList Db::mSoftDelete = QStringList();
static QThreadStorage<QSqlDatabase> mDatabasePool;
#ifdef QT_DEBUG
bool Db::DEBUG = true;
#else
bool Db::DEBUG = false;
#endif

Db::Db():
    parent(NULL)
{
    mDebug = DEBUG;
}

Db::~Db()
{
    if(parent == NULL) {
        for(int i = 0; i < childs.size(); i++)
            delete childs[i];
    }
}

Db *Db::createInstance(bool checkDBName, bool newConnection, QString *err)
{
    Db* db = new Db();
    if(db->init(DBSETTING.host, DBSETTING.port, DBSETTING.username, DBSETTING.password, DBSETTING.dbName, checkDBName, newConnection))
        return db;
    qCritical() << TAG << "Error openning database connection :" << db->lastError().text();
    if(err != nullptr) *err = db->lastError().text();
    delete db;
    return nullptr;
}

bool Db::setDbSetting(const QString &host, int port, const QString &username, const QString &password, const QString &dbname)
{
    DBSETTING.set(host, port, username, password, dbname);
    return true;
    /*auto db = createInstance();
    bool ret = db != nullptr;
    if(db) delete db;
    return ret;*/
}

bool Db::checkConnection(QString &error)
{
    QString err;
    auto db = createInstance(true, false, &err);
    if(db == nullptr) {
        error = err;
        return false;
    }
    delete db;
    return true;
}

void Db::setDatabaseType(const QString &db)
{
    DBTYPE = db;
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
        mSelect.append(QStringLiteral(","));
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
        mWhere.append(QStringLiteral(" AND "));
    if(!val.isValid())
        mWhere.append(value);
    else {
        mWhere.append(value);
        if(val.type() == QVariant::String)
            mWhere.append(QStringLiteral("'") + val.toString() + QStringLiteral("'"));
        else
            mWhere.append(QString::number(val.toInt()));
    }
    return this;
}

Db *Db::like(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QStringLiteral(" AND "));
    mWhere.append(value);
    if(val.startsWith(QStringLiteral("%")) || val.endsWith(QStringLiteral("%"))) {
        mWhere.append(QStringLiteral(" LIKE '"));
        mWhere.append(val);
        mWhere.append(QStringLiteral("'"));
    } else {
        mWhere.append(QStringLiteral(" LIKE '%"));
        mWhere.append(val);
        mWhere.append(QStringLiteral("%'"));
    }
    return this;
}

Db *Db::likePost(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QStringLiteral(" AND "));
    mWhere.append(value);
    mWhere.append(QStringLiteral(" LIKE '"));
    mWhere.append(val);
    mWhere.append(QStringLiteral("%'"));
    return this;
}

Db *Db::likeNative(const QString &value, const QString &val)
{
    if (!mWhere.isEmpty())
        mWhere.append(QStringLiteral(" AND "));
    mWhere.append(value);
    mWhere.append(QStringLiteral(" LIKE '"));
    mWhere.append(val);
    mWhere.append(QStringLiteral("'"));
    return this;
}

Db *Db::whereOr(const QString &value)
{
    if (!mWhere.isEmpty())
        mWhere.append(QStringLiteral(" OR "));
    mWhere.append(value);
    return this;
}

Db *Db::join(const QString &value)
{
    if(!mJoin.isEmpty())
        mJoin.append(QStringLiteral(" "));
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
        mSort.append(QStringLiteral(", "));
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
    QString sql = QString(QStringLiteral("SELECT "));
    /*if(mSoftDelete.contains(mTable))
        where(QStringLiteral("deleted = 0"));*/
    if(mSelect.isEmpty())
        mSelect.append(QStringLiteral("*"));
    if(!select.isEmpty())
        sql.append(select);
    else
        sql.append(mSelect);
    sql.append(QStringLiteral(" FROM ")).append(mTable).append(QStringLiteral(" "));
    if(!mJoin.isEmpty())
        sql.append(mJoin);
    if(!mWhere.isEmpty())
        sql.append(QStringLiteral(" WHERE ")).append(mWhere);
    if(!mGroup.isEmpty())
        sql.append(QStringLiteral(" GROUP BY ")).append(mGroup);
    if(!mSort.isEmpty())
        sql.append(QStringLiteral(" ORDER BY ")).append(mSort);
    if(!mLimit.isEmpty())
        sql.append(QStringLiteral(" LIMIT ")).append(mLimit);
    if(!mStart.isEmpty())
        sql.append(QStringLiteral(" OFFSET ")).append(mStart);
    if(mDebug)
        qDebug() << TAG << sql;
    return sql;
}

QSqlError Db::lastError() { return mLastError; }

QVariant Db::lastInsertedId() { return mInsertedId; }

QString Db::lastQuery() { return mLastQuery; }

DbResult Db::exec()
{
    QList<QVariant> result;
    QSqlQuery query(getDatabase());
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

DbResult Db::execResult(const QString &sql)
{
    QList<QVariant> result;
    QSqlQuery query(getDatabase());
    if (query.exec(sql)) {
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

DbResult Db::get(const QString &tableName)
{
    return table(tableName)->exec();
}

bool Db::exec(const QString &sqlcommand)
{
    QSqlQuery query(getDatabase());
    bool ok = query.exec(sqlcommand);
    reset();
    postQuery(&query);
    if(mDebug)
        qDebug() << TAG << sqlcommand;
    if(!ok) {
        qCritical() << TAG << lastQuery();
        qCritical() << TAG << mLastError.text();
    }
    return ok;
}

bool Db::insert(const QString &table, const QVariantMap &data)
{
    QSqlQuery query(getDatabase());
    QString sql = QString(QStringLiteral("INSERT INTO "));
    QString values = QString(QStringLiteral("VALUES ("));
    sql.append(table).append(QStringLiteral(" ("));
    const QList<QString> &keys = data.keys();
    for(int i = 0; i < keys.size(); i++) {
        sql.append(keys.at(i));
        values.append(QStringLiteral("?"));
        if(i != keys.size() - 1) {
            sql.append(QStringLiteral(","));
            values.append(QStringLiteral(","));
        }
    }
    sql.append(QStringLiteral(") ")).append(values).append(QStringLiteral(")"));
    query.prepare(sql);
    for(int i = 0; i < keys.size(); i++)
        query.bindValue(i, data[keys.at(i)]);
    reset();
    bool res = query.exec();
    if(mDebug)
        qDebug() << TAG << query.lastQuery() << dataToString(data);
    postQuery(&query);
    if(!res) {
        qCritical() << TAG << query.lastQuery() << dataToString(data);
        qCritical() << TAG << mLastError.text();
    }
    return res;
}

bool Db::update(const QString &table, const QVariantMap &data)
{
    QSqlQuery query(getDatabase());
    QString sql = QString(QStringLiteral("UPDATE "));
    sql.append(table).append(QStringLiteral(" SET "));
    if(mUpdate.contains(table)) {
        sql.append(QStringLiteral("updated_at = NOW() "));
    }
    const QList<QString> &keys = data.keys();
    for(int i = 0; i < keys.size(); i++) {
        sql.append(keys.at(i));
        if(data.value(keys.at(i)).type() != QVariant::Invalid) {
            sql.append(QStringLiteral(" = ?"));
        }
        if(i != keys.size() - 1)
            sql.append(QStringLiteral(","));
    }
    sql.append(QStringLiteral(" WHERE ")).append(mWhere);
    query.prepare(sql);
    for(int i = 0; i < keys.size(); i++)
        query.bindValue(i, data[keys.at(i)]);
    reset();
    bool res = query.exec();
    if(mDebug)
        qDebug() << TAG << query.lastQuery() << dataToString(data);
    postQuery(&query);
    if(!res) {
        qCritical() << TAG << query.lastQuery() << dataToString(data);
        qCritical() << TAG << mLastError.text();
    }
    return res;
}

bool Db::del(const QString &table)
{
    if(mSoftDelete.contains(table)) {
        QVariantMap map;
        map.insert(QStringLiteral("deleted"), 1);
        return update(table, map);
    } else {
        QSqlQuery query(getDatabase());
        QString sql = QString(QStringLiteral("DELETE FROM "));
        sql.append(table).append(QStringLiteral(" WHERE ")).append(mWhere);
        reset();
        bool res = query.exec(sql);
        if(mDebug)
            qDebug() << TAG << query.lastQuery();
        postQuery(&query);
        if(!res) {
            qCritical() << TAG << query.lastQuery();
            qCritical() << TAG << mLastError.text();
        }
        return res;
    }
}

int Db::count()
{
    QSqlQuery query(getDatabase());
    Db *db = clone();
    QString sql = db->getSelectQuery(QStringLiteral("count(*)"));
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
    return getDatabase().transaction();
}

bool Db::commit()
{
    return getDatabase().commit();
}

bool Db::roolback()
{
    return getDatabase().rollback();
}

bool Db::isSQLite()
{
    return DBTYPE.compare("SQLITE") == 0;
}

bool Db::truncateTable(const QString &table)
{
    if(isSQLite()) {
        this->exec(QString("DELETE FROM %1").arg(table));
    } else {
        this->exec(QString("TRUNCATE TABLE %1").arg(table));
    }
}

QSqlDatabase Db::getDatabase()
{
    if(mDatabasePool.hasLocalData()) {
        return mDatabasePool.localData();
    } else {
        auto database = QSqlDatabase::addDatabase(SQLDRIVERNAME[DBTYPE], QUuid::createUuid().toString());
        mDatabasePool.setLocalData(database);
        return database;
    }
}

bool Db::init(const QString &host, int port, const QString &username, const QString &password, const QString &dbname, bool checkDBName, bool newConnection)
{
    auto database = getDatabase();
    if(database.isOpen()) {
        if(newConnection) database.close();
        else {
            mSupportTransaction = database.driver()->hasFeature(QSqlDriver::Transactions);
            return true;
        }
    }
    bool ret = false;
    if(DBTYPE == "MYSQL") {
        database.setPort(port);
        database.setHostName(host);
        database.setUserName(username);
        database.setPassword(password);
        if(checkDBName) {
            database.open();
            database.exec(QString("CREATE DATABASE IF NOT EXISTS %1").arg(dbname));
            database.close();
        }
        database.setDatabaseName(dbname);
        ret = database.open();
    } else if(DBTYPE == "SQLITE") {
        QString dirpath = Preference::getString(SETTING::SQLITE_DBPATH);
        QString dbname = Preference::getString(SETTING::SQLITE_DBNAME);
        QDir dir = QDir::home();
        if(dbname.isEmpty()) dbname = "sultan.db";
        if(!dbname.endsWith(".db")) dbname += ".db";
        if(dirpath.isEmpty()) {
            dir.mkdir(".sultan");
            dir.cd(".sultan");
        } else {
            dir.setPath(dirpath);
        }
        bool reset = Preference::getBool(SETTING::RESETDB, false);
        if(reset) {
            QFile::remove(dir.absoluteFilePath(dbname));
            Preference::setValue(SETTING::RESETDB, false);
        }
        database.setDatabaseName(dir.absoluteFilePath(dbname));
        qDebug() << TAG << "SQLite database path :" << dir.absoluteFilePath(dbname);
        ret = database.open();
    }
    mSupportTransaction = database.driver()->hasFeature(QSqlDriver::Transactions);
    qDebug() << TAG << "Database support transaction :" << mSupportTransaction;
    if(!ret) mLastError = database.lastError();
    reset();
    return ret;
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
    str.append(QStringLiteral("{"));
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        str.append(i.key()).append(QStringLiteral(": ")).append(i.value().toString()).append(QStringLiteral(","));
    }
    str.append(QStringLiteral("}"));
    return str;
}
