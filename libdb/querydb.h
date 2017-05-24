/*
 * querydb.h
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

#ifndef QUERYDB_H
#define QUERYDB_H

#include "db_global.h"

#include <QString>
#include <QMap>
#include <QVariant>

namespace LibG {
class Message;
}

struct FilterData {
    FilterData(int type, const QVariant &data)
    {
        this->type = type;
        this->data = data;
    }

    int type;
    QVariant data;
};

namespace LibDB {

class DBSHARED_EXPORT QueryDB
{
public:
    QueryDB();
    void setFilter(const QString &key, int type, const QVariant &data);
    void addData(const QString &key, const QVariant &d);
    void clearFilter();
    void clearAll();
    void clearData();
    void clearSort();
    void removeFilter(const QString &key);
    void setSort(const QString &sort);
    void setLimit(int val);
    void setStart(int val);
    inline int getLimit() { return mLimit; }
    void bind(LibG::Message *msg);

private:
    QMap<QString, FilterData> mFilter;
    QMap<QString, QVariant> mData;
    QString mSort;
    int mLimit;
    int mStart;
};

}
#endif // QUERYDB_H
