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
    mHeaders << "No" << "Name" << "Count" << "Price" << "Total";
}

CashierTableModel::~CashierTableModel()
{
    qDeleteAll(mData);
}

int CashierTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mData.size();
}

int CashierTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return mHeaders.size();
}

QVariant CashierTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return mHeaders[section];
    return QVariant();
}

QVariant CashierTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole) {
        auto item = mData[index.row()];
        switch(index.column()) {
        case 0: return index.row() + 1;
        case 1: return item->name;
        case 2: return QLocale().toString(item->count);
        case 3: return QLocale().toString(item->price);
        case 4: return QLocale().toString(item->total);
        }
    } else if(role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case 0: return QVariant(Qt::AlignVCenter | Qt::AlignHCenter);
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
    CashierItem *item = nullptr;
    if(row >= 0 && row < mData.size())
        item = mData[row];
    return createIndex(row, column, item);
}

void CashierTableModel::addItem(float count, const QString &name, const QString &barcode, const QVariantList &prices)
{
    float totCount = getTotalCount(barcode) + count;
    int row = rowOfBarcode(barcode);
    if(!prices.isEmpty()) mPrices[barcode] = prices;
    if(totCount <= 0 && row >= 0) {
        beginRemoveRows(QModelIndex(), row, row);
        mData.removeAt(row);
        endRemoveRows();
    } else if(totCount > 0) {
        if(row >= 0) {
            mData[row]->count = totCount;
            calculatePrices(mData[row]);
            emit dataChanged(createIndex(row, 0), createIndex(row, mHeaders.size() - 1));
            emit selectRow(createIndex(row, 0, mData[row]));
        } else {
            beginInsertRows(QModelIndex(), mData.size(), mData.size());
            auto item = new CashierItem(name, barcode, totCount, 0, 0);
            calculatePrices(item);
            mData.append(item);
            endInsertRows();
            emit selectRow(createIndex(mData.size() - 1, 0, mData[mData.size() - 1]));
        }
    }
    calculateTotal();
}

void CashierTableModel::reset()
{
    beginResetModel();
    qDeleteAll(mData);
    mData.clear();
    mPrices.clear();
    endResetModel();
    calculateTotal();
}

float CashierTableModel::getTotalCount(const QString &barcode)
{
    float retVal = 0;
    for(auto item : mData) {
        if(item->type == CashierItem::Item && !item->barcode.compare(barcode))
            retVal += item->count;
    }
    return retVal;
}

void CashierTableModel::calculateTotal()
{
    double total = 0;
    for(auto item : mData)
        total += item->total;
    emit totalChanged(total);
}

int CashierTableModel::rowOfBarcode(const QString &barcode)
{
    for(int i = 0; i < mData.size(); i++)
        if(!mData[i]->barcode.compare(barcode))
            return i;
    return -1;
}

void CashierTableModel::calculatePrices(CashierItem *item)
{
    const QVariantList &prices = mPrices[item->barcode];
    float count = item->count;
    item->total = 0;
    if(prices.size() == 1) {
        item->price = prices[0].toMap()["price"].toDouble();
        item->total = item->count * item->price;
    } else {
        for(int i = prices.count() - 1; i >= 0; i--) {
            const QVariantMap &p = prices[i].toMap();
            double price = p["price"].toDouble();
            float c = p["count"].toFloat();
            if(c <= count) {
                if(i == 0) {
                    item->total += count * price;
                } else {
                    float temp = std::fmod(count, c);
                    item->total += (count - temp) * price;
                    count -= (count - temp);
                    if(count <= 0.0f) break;
                }
            }
        }
        item->price = item->total / item->count;
    }
}
