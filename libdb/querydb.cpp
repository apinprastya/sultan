/*
 * querydb.cpp
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

#include "querydb.h"

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

