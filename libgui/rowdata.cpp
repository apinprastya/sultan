/*
 * rowdata.cpp
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
#include "rowdata.h"
#include "global_constant.h"
#include "tableitem.h"

using namespace LibGUI;

RowData::RowData() {}

RowData::~RowData() { clearAndRelease(); }

void RowData::clearAndRelease() {
    qDeleteAll(mData);
    mData.clear();
    mOffset.clear();
}

void RowData::insert(int index, TableItem *item) {
    if (index == 0) {
        QMap<int, int> temp = mOffset;
        const QList<int> &keys = temp.keys();
        mOffset.clear();
        for (int i = 0; i < keys.size(); i++) {
            if (i == 0)
                mOffset.insert(0, temp[keys[i]] + 1);
            else
                mOffset.insert(keys[i] + 1, temp[keys[i]]);
        }
        mData.prepend(item);
    }
}

void RowData::insert(int index, const QList<TableItem *> &list) {
    int offset = 0;
    const QList<int> &keys = mOffset.keys();
    for (int i = 0; i < keys.size(); i++) {
        if (index < keys[i])
            break;
        offset = keys[i];
    }
    if ((index - offset) <= mOffset[offset]) {
        mOffset[offset] = mOffset[offset] + list.size();
        for (int i = list.count() - 1; i >= 0; i--)
            mData.insert(index, list[i]);
    } else {
        mOffset.insert(index, list.size());
        int idx = getInternalIndex(offset) + mOffset[offset];
        for (int i = list.size() - 1; i >= 0; i--) {
            mData.insert(idx, list[i]);
        }
    }
}

void RowData::append(TableItem *item) { mData.append(item); }

void RowData::removeAndRelease(int index) {
    int real = getInternalIndex(index);
    if (real >= 0) {
        if (!mOffset.isEmpty()) {
            QMapIterator<int, int> i(mOffset);
            int lenght = 0;
            int key = 0;
            while (i.hasNext()) {
                i.next();
                if (lenght + i.value() > real) {
                    break;
                }
                key = i.key();
            }
            mOffset[key] = mOffset[key] - 1;
        }
        auto item = mData.takeAt(real);
        delete item;
    }
}

TableItem *RowData::operator[](int index) {
    int real = getInternalIndex(index);
    if (real >= 0 && !mData.isEmpty() && real >= 0 && real < mData.size())
        return mData[real];
    return nullptr;
}

TableItem *RowData::at(int index) {
    int real = getInternalIndex(index);
    if (real >= 0 && !mData.isEmpty() && real >= 0 && real < mData.size())
        return mData[real];
    return nullptr;
}

TableItem *RowData::operator[](int index) const {
    const int real = getInternalIndex(index);
    if (real >= 0 && !mData.isEmpty() && real >= 0 && real < mData.size())
        return mData[real];
    return nullptr;
}

bool RowData::exist(int index) const {
    int real = getInternalIndex(index);
    return real >= 0;
}

int RowData::getIndexOfId(const QString &key, const QVariant &id) {
    for (int i = 0; i < mData.size(); i++) {
        if (mData[i]->data(key) == id) {
            return getLogicalIndex(i);
        }
    }
    return -1;
}

int RowData::getLogicalIndex(int internalIndex) {
    if (mOffset.isEmpty())
        return internalIndex;
    if (internalIndex >= 0) {
        QMapIterator<int, int> i(mOffset);
        int lenght = 0;
        while (i.hasNext()) {
            i.next();
            if (lenght + i.value() > internalIndex) {
                return i.key() + internalIndex - lenght;
            }
            lenght += i.value();
        }
        return mOffset.lastKey() + internalIndex - lenght;
    }

    return -1;
}

TableItem *RowData::getItem(const QString &key, const QVariant &val) {
    for (int i = 0; i < mData.size(); i++) {
        if (mData[i]->data(key) == val) {
            return mData[i];
        }
    }
    return nullptr;
}

int RowData::indexOf(TableItem *item) {
    for (int i = 0; i < mData.size(); i++) {
        if (mData[i] == item) {
            return getLogicalIndex(i);
        }
    }
    return -1;
}

int RowData::getInternalIndex(int index) const {
    const QList<int> &keys = mOffset.keys();
    int offset = 0;
    int real = index;
    int hold = 0;
    int lastlength = LibG::CONFIG::ITEMS_PER_LOAD;
    if (!mOffset.isEmpty())
        lastlength = mOffset.first();
    for (int i = 0; i < keys.size(); i++) {
        if (index < keys[i])
            break;
        lastlength = mOffset[keys[i]];
        real = index - keys[i];
        offset += hold;
        hold = mOffset[keys[i]];
    }
    if (real >= lastlength)
        return -1;
    return offset + real;
}
