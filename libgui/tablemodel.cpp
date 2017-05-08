/*
 * tablemodel.cpp
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
#include "tablemodel.h"
#include "global_constant.h"
#include "tableitem.h"
#include "rowdata.h"

using namespace LibGUI;

TableModel::TableModel(QObject *parent):
    QAbstractTableModel(parent),
    mNumRow(0)
{

}

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mNumRow;
}

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return mHeaders.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(role == Qt::DisplayRole) {
        const int row = index.row();
        if(!mData.exist(row + LibG::CONFIG::ITEMS_PER_LOAD))
            emit loadMore((row + LibG::CONFIG::ITEMS_PER_LOAD) / LibG::CONFIG::ITEMS_PER_LOAD);
        if((row - LibG::CONFIG::ITEMS_PER_LOAD) > 0 && !mData.exist(row - LibG::CONFIG::ITEMS_PER_LOAD))
            emit loadMore((row - LibG::CONFIG::ITEMS_PER_LOAD) / LibG::CONFIG::ITEMS_PER_LOAD);
        if(!mData.exist(row)) {
            emit loadMore(row / LibG::CONFIG::ITEMS_PER_LOAD);
            return QVariant(QLatin1String("loading..."));
        }
        auto item = mData[row];
        return item->data(mColumns[index.column()]);
    } else if(role == Qt::TextAlignmentRole) {
        return mAlignments[index.column()];
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return mHeaders[section];
    return QVariant();
}

void TableModel::reset()
{
    beginResetModel();
    mData.clearAndRelease();
    mNumRow = 0;
    endResetModel();
}

void TableModel::addColumn(const QString &key, const QString &title, int align)
{
    mColumns.push_back(key);
    mHeaders.push_back(title);
    mAlignments.push_back(align);
}
