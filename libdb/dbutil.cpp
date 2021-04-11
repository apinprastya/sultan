/*
 * dbutil.cpp
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

#include "dbutil.h"
#include "dbresult.h"
#include <QDateTime>
#include <QStringBuilder>

using namespace LibDB;

static QStringList format{"yyyy-MM-dd", "yyyy-MM-ddThh:mm:ss", "yyyy-MM-dd hh:mm:ss", "yyyy-MM-ddThh:mm:ss.zzz"};

DBUtil::DBUtil() {}

QString DBUtil::formatDateToIndo(const QString &d, bool useTime) {
    if (d.isEmpty())
        return d;
    auto date = QDateTime::fromString(d, QLatin1String("yyyy-MM-ddThh:mm:ss"));
    if (useTime)
        return date.toString(QLatin1String("dd-MM-yyyy hh:mm:ss"));
    return date.toString(QLatin1String("dd-MM-yyyy"));
}

QDate DBUtil::sqlDateToDate(const QString &value) {
    for (const QString &s : format) {
        auto date = QDate::fromString(value, s);
        if (date.isValid())
            return date;
    }
    return QDate();
}

QDateTime DBUtil::sqlDateToDateTime(const QString &value) {
    for (const QString &s : format) {
        auto date = QDateTime::fromString(value, s);
        if (date.isValid())
            return date;
    }
    return QDateTime();
}

QString DBUtil::DBResultToString(DbResult *result) {
    QString str;
    for (int i = 0; i < result->size(); i++) {
        // str = str %
    }
    return str;
}
