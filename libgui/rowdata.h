/*
 * rowdata.h
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
#ifndef ROWDATA_H
#define ROWDATA_H

#include "gui_global.h"
#include <QVariant>
#include <QList>
#include <QMap>

namespace LibGUI  {

class TableItem;

class GUISHARED_EXPORT RowData
{
public:
    RowData();
    ~RowData();
    void clearAndRelease();
    void insert(int index, TableItem *item);
    void insert(int index, const QList<TableItem*> &list);
    void append(TableItem *item);
    void removeAndRelease(int index);
    TableItem *operator[](int index);
    TableItem *at(int index);
    TableItem *operator[](int index) const;
    bool exist(int index) const;
    int getIndexOfId(const QString &key, const QVariant &id);
    inline int size() { return mData.size(); }
    inline TableItem *getItem(int index) { return mData[index]; }
    int getLogicalIndex(int internalIndex);
    TableItem *getItem(const QString &key, const QVariant &val);
    int indexOf(TableItem *item);

private:
    QList<TableItem*> mData;
    //key: index, value: lenght
    QMap<int, int> mOffset;

    int getInternalIndex(int index) const;
};

}
#endif // ROWDATA_H
