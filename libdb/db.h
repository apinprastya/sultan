/*
 * db.h
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

#ifndef DB_H
#define DB_H

#include "db_global.h"
#include "dbresult.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

namespace LibDB {

struct DBSetting {
    DBSetting() {}
    DBSetting(const QString &host, int port, const QString &username, const QString &password, const QString &dbname)
        : port(port), host(host), username(username), password(password), dbName(dbname) {}
    void set(const QString &host, int port, const QString &username, const QString &password, const QString &dbname) {
        this->port = port;
        this->host = host;
        this->username = username;
        this->password = password;
        this->dbName = dbname;
    }
    int port;
    QString host;
    QString username;
    QString password;
    QString dbName;
};

class DBSHARED_EXPORT Db {
  private:
    Db();

  public:
    ~Db();
    static Db *createInstance(bool checkDBName = false, bool newConnection = false, QString *err = nullptr);
    static bool setDbSetting(const QString &host, int port, const QString &username, const QString &password,
                             const QString &dbname);
    static bool checkConnection(QString &error);
    static void setDatabaseType(const QString &db);
    Db *reset();
    Db *select(const QString &value);
    Db *table(const QString &value);
    Db *where(const QString &value, const QVariant &val = QVariant());
    Db *like(const QString &value, const QString &val);
    Db *likePost(const QString &value, const QString &val);
    Db *likeNative(const QString &value, const QString &val);
    Db *whereOr(const QString &value);
    Db *join(const QString &value);
    Db *limit(int limit);
    Db *start(int start);
    Db *sort(const QString &sort);
    Db *group(const QString &group);
    QString getSelectQuery(const QString &select = QString());
    QSqlError lastError();
    QVariant lastInsertedId();
    QString lastQuery();
    DbResult exec();
    DbResult execResult(const QString &sql);
    DbResult get(const QString &tableName);
    bool exec(const QString &sqlcommand);
    bool insert(const QString &table, const QVariantMap &data);
    bool update(const QString &table, const QVariantMap &data);
    bool del(const QString &table);
    int count();
    Db *clone();
    bool beginTransaction();
    bool commit();
    bool roolback();
    inline bool isSupportTransaction() { return mSupportTransaction; }
    inline Db *clearSelect() {
        mSelect.clear();
        return this;
    }
    bool isSQLite();
    bool truncateTable(const QString &table);

  private:
    static QStringList mCreated;
    static QStringList mUpdate;
    static QStringList mSoftDelete;
    static bool DEBUG;
    bool mSupportTransaction = false;
    QString mWhere;
    QString mSelect;
    QString mJoin;
    QString mTable;
    QString mLimit;
    QString mStart;
    QString mSort;
    QString mGroup;
    QList<Db *> childs;
    Db *parent;
    QString mLastQuery;
    QVariant mInsertedId;
    QSqlError mLastError;

    static QSqlDatabase getDatabase();
    bool init(const QString &host, int port, const QString &username, const QString &password, const QString &dbname,
              bool checkDBName = false, bool newConnection = false);
    void postQuery(QSqlQuery *query);
    QString dataToString(const QVariantMap &map);

  protected:
    bool mDebug;
};

} // namespace LibDB
#endif // DB_H
