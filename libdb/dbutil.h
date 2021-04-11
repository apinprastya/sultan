/*
 * dbutil.h
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

#ifndef DBUTIL_H
#define DBUTIL_H

#include "db_global.h"
#include <QDateTime>
#include <QString>

namespace LibDB {

class DbResult;

class DBSHARED_EXPORT DBUtil {
  public:
    DBUtil();
    static QString formatDateToIndo(const QString &d, bool useTime = true);
    static QDate sqlDateToDate(const QString &value);
    static QDateTime sqlDateToDateTime(const QString &value);
    static QString DBResultToString(DbResult *result);
};

} // namespace LibDB

#endif // DBUTIL_H
