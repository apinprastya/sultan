/*
 * cashiertablemodel.cpp
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
#include "cashiertablemodel.h"
#include "cashieritem.h"
#include <QLocale>

using namespace LibGUI;

CashierTableModel::CashierTableModel(QObject *parent):
    QAbstractTableModel(parent)
{
    mHeader << "Name" << "count" << "price" << "total";
}

int CashierTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mData.size();
}

int CashierTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return mHeader.size();
}

QVariant CashierTableModel::headerData(int section, Qt::Orientation orientation, int /*role*/) const
{
    if(orientation == Qt::Horizontal)
        return mHeader[section];
    return QVariant();
}

QVariant CashierTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole) {
        auto item = mData[index.column()];
        switch(index.column()) {
        case 0: return item->barcode;
        case 1: return QLocale().toString(item->count);
        case 2: return QLocale().toString(item->price);
        case 3: return QLocale().toString(item->total);
        }
    } else if(role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case 2:
        case 3:
        case 4:
            return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        default:
            return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
        }
    }
    return QVariant();
}

QModelIndex CashierTableModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    return createIndex(row, column, mData[row]);
}
