/*
 * querydb.cpp
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

#include "querydb.h"
#include "message.h"

using namespace LibDB;

QueryDB::QueryDB():
    mLimit(0),
    mStart(0)
{
}

void QueryDB::setFilter(const QString &key, int type, const QVariant &data)
{
    FilterData filter(type, data);
    mFilter.insert(key, filter);
}

void QueryDB::addData(const QString &key, const QVariant &d)
{
    mData.insert(key, d);
}

void QueryDB::clearFilter()
{
    mFilter.clear();
}

void QueryDB::clearAll()
{
    mFilter.clear();
    mData.clear();
    mSort = QString();
    mStart = 0;
    mLimit = 0;
}

void QueryDB::clearData()
{
    mData.clear();
}

void QueryDB::clearSort()
{
    mSort = QString();
}

void QueryDB::removeFilter(const QString &key)
{
    if(mFilter.contains(key))
        mFilter.remove(key);
}

void QueryDB::setSort(const QString &sort)
{
    mSort = sort;
}

void QueryDB::setLimit(int val)
{
    mLimit = val;
}

void QueryDB::setStart(int val)
{
    mStart = val;
}

void QueryDB::bind(LibG::Message *msg)
{
    msg->resetQuery();
    msg->setSort(mSort);
    if(mLimit > 0)
        msg->setLimit(mLimit);
    if(mStart > 0)
        msg->setStart(mStart);
    QMapIterator<QString, FilterData> i(mFilter);
    while (i.hasNext()) {
        i.next();
        const FilterData &d = i.value();
        msg->addFilter(i.key(), d.type, d.data);
    }
    QMapIterator<QString, QVariant> d(mData);
    while (d.hasNext()) {
        d.next();
        msg->addData(d.key(), d.value());
    }
}

