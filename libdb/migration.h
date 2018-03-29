/*
 * migration.cpp
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

#ifndef MIGRATION_H
#define MIGRATION_H

#include "db_global.h"
#include <QDir>
#include <functional>

namespace LibDB {

class Db;

class DBSHARED_EXPORT Migration
{
public:
    static bool migrateAll(const QStringList &filelist, const QString &dbtype, std::function<bool(const QString &)> afterCallback = nullptr);
    static void setAfterMigrate(std::function<bool(const QString &)> afterCallback);
    Migration(Db *db, const QStringList &filelist, const QString &dbtype);

private:
    Db *mDb;
    QStringList mFileNameList;
    QString mLastFile;
    QString mDbType;
    std::function<bool(const QString &)> mAfterMigrate = nullptr;

    bool migrate();
    void init();
    bool executeFile(const QString &filePath);
};

}

#endif // MIGRATION_H
