/*
 * tablemodel.cpp
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
#include "tablemodel.h"
#include "global_constant.h"
#include "tableitem.h"
#include "rowdata.h"
#include "message.h"
#include "preference.h"
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

TableModel::TableModel(QObject *parent):
    QAbstractTableModel(parent),
    mNumRow(0)
{
    mIsLoaded = false;
    mQuery.setLimit(LibG::CONFIG::ITEMS_PER_LOAD);
    connect(this, SIGNAL(loadMore(int)), SLOT(loadPage(int)));
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
        if(mFormater.contains(mColumns[index.column()]))
            return mFormater[mColumns[index.column()]](item, mColumns[index.column()]);
        return item->data(mColumns[index.column()]);
    } else if(role == Qt::TextAlignmentRole) {
        return mAlignments[index.column()];
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return mHeaders[section];
    }
    return QVariant();
}

QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    auto item = mData[row];
    return createIndex(row, column, item);
}

void TableModel::reset()
{
    beginResetModel();
    mIsLoaded = false;
    mData.clearAndRelease();
    mNumRow = 0;
    mPageStatus.clear();
    endResetModel();
}

void TableModel::addColumn(const QString &key, const QString &title, const int &align, std::function<QVariant(TableItem *, const QString&)> formater)
{
    mColumns.push_back(key);
    mHeaders.push_back(title);
    mAlignments.push_back(align | Qt::AlignVCenter);
    if(formater != nullptr) mFormater.insert(key, formater);
}

void TableModel::addColumnMoney(const QString &key, const QString &title)
{
    addColumn(key, title, Qt::AlignRight, [](TableItem *item, const QString &key) -> QVariant {
        return Preference::toString(item->data(key).toDouble());
    });
}

void TableModel::setFilter(const QString &key, int type, const QVariant &value)
{
    mQuery.setFilter(key, type, value);
}

void TableModel::clearFilter()
{
    mQuery.clearFilter();
}

void TableModel::refresh()
{
    reset();
    loadPage();
}

void TableModel::messageReceived(LibG::Message *msg)
{
    if(msg->status() == LibG::STATUS::OK) {
        if(msg->command() == std::get<1>(mTypeCommand))
            readData(msg);
    }
}

void TableModel::loadPage(int page)
{
    if(page != 0 && mQuery.getLimit() <= 0) return;
    if(mPageStatus.value(page) != None) return;
    mPageStatus[page] = Loading;
    LibG::Message msg(std::get<0>(mTypeCommand), std::get<1>(mTypeCommand));
    mQuery.setStart(page * LibG::CONFIG::ITEMS_PER_LOAD);
    mQuery.bind(&msg);
    sendMessage(&msg);
}

void TableModel::readData(LibG::Message *msg)
{
    int num = msg->data(QStringLiteral("total")).toInt();
    int start = msg->data(QStringLiteral("start")).toInt();
    if(!mIsLoaded) {
        beginResetModel();
        mData.clearAndRelease();
    }

    const QVariantList &l = msg->data(QStringLiteral("data")).toList();
    QList<TableItem*> items;
    for(int i = 0; i < l.size(); i++) {
        auto item = new TableItem();
        item->fill(l.at(i).toMap());
        items.append(item);
    }
    mData.insert(start, items);
    if(!mIsLoaded) {
        mNumRow = num;
        endResetModel();
        emit firstDataLoaded();
    }
    if(mIsLoaded)
        emit dataChanged(createIndex(start, 0), createIndex(start + l.size(), mHeaders.size()));
    mIsLoaded = true;
}
