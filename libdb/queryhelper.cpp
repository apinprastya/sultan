/*
 * queryhelper.cpp
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

#include "queryhelper.h"
#include "db_constant.h"

using namespace LibDB;

QueryHelper::QueryHelper()
{
}

Db *QueryHelper::filter(Db *db, const QVariantMap &data, const QMap<QString, QString> &fieldMap)
{
    if(data.contains(QLatin1String("filter")) && data[QLatin1String("filter")].type() == QVariant::Map) {
        const QVariantMap &d = data[QLatin1String("filter")].toMap();
        QMapIterator<QString, QVariant> i(d);
        while (i.hasNext()) {
            i.next();
            QString key = i.key();
            if(key.contains(QLatin1String("$")))
                key = key.mid(2);
            if(fieldMap.contains(key))
                key = fieldMap[key];
            const QVariantMap &option = i.value().toMap();
            int type = option[QLatin1String("type")].toInt();
            if(type == COMPARE::IGNORE)
                continue;
            if(type == COMPARE::ISNULL || type == COMPARE::ISNOTNULL) {
                db->where(key + getSign(type));
            } else if(type == COMPARE::LIKE) {
                db->like(key, option[QLatin1String("value")].toString());
            } else if(type == COMPARE::LIKE_NATIVE) {
                db->likeNative(key, option[QLatin1String("value")].toString());
            } else {
                db->where(key + getSign(type), option[QLatin1String("value")]);
            }
        }
    }
    return db;
}

Db *QueryHelper::sort(Db *db, const QVariantMap &data)
{
    if(data.contains(QLatin1String("sort"))) {
        const QString &sort = data["sort"].toString();
        if(!sort.isEmpty())
            db->sort(data[QLatin1String("sort")].toString());
    }
    return db;
}

Db *QueryHelper::limitOffset(Db *db, const QVariantMap &data)
{
    if(data.contains(QLatin1String("limit"))) {
        int limit = data[QLatin1String("limit")].toInt();
        if(limit > 0)
            db->limit(limit);
    }
    if(data.contains(QLatin1String("start"))) {
        db->start(data[QLatin1String("start")].toInt());
    }
    return db;
}

QString QueryHelper::getSign(int type)
{
    switch(type) {
        case COMPARE::EQUAL:
            return QLatin1String("=");
        case COMPARE::NEQUAL:
            return QLatin1String("!=");
        case COMPARE::LESS:
            return QLatin1String("<");
        case COMPARE::GREATER:
            return QLatin1String(">");
        case COMPARE::LESS_EQUAL:
            return QLatin1String("<=");
        case COMPARE::GREATER_EQUAL:
            return QLatin1String(">=");
        case COMPARE::ISNULL:
            return QLatin1String(" IS NULL ");
        case COMPARE::ISNOTNULL:
            return QLatin1String(" IS NOT NULL ");
        case COMPARE::LIKE:
            return QLatin1String(" LIKE ");
    }
    return QLatin1String("");
}
