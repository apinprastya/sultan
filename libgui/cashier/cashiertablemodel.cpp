/*
 * cashiertablemodel.cpp
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
#include "cashiertablemodel.h"
#include "cashieritem.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "message.h"
#include "preference.h"
#include "util.h"
#include <QDebug>
#include <QLocale>
#include <cmath>

using namespace LibGUI;
using namespace LibG;

static CashierTableModel *sInstance = nullptr;

CashierTableModel::CashierTableModel(MessageBus *bus, QObject *parent) : QAbstractTableModel(parent) {
    setMessageBus(bus);
    mHeaders << "No" << tr("Barcode") << tr("Name") << tr("Count") << tr("Unit") << tr("Price") << tr("Discount")
             << tr("Total");
    sInstance = this;
}

CashierTableModel::~CashierTableModel() { qDeleteAll(mData); }

CashierTableModel *CashierTableModel::instance() { return sInstance; }

int CashierTableModel::rowCount(const QModelIndex & /*parent*/) const { return mData.size(); }

int CashierTableModel::columnCount(const QModelIndex & /*parent*/) const { return mHeaders.size(); }

QVariant CashierTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return mHeaders[section];
    return QVariant();
}

QVariant CashierTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        auto item = mData[index.row()];
        switch (index.column()) {
        case 0:
            return index.row() + 1;
        case 1:
            return item->barcode;
        case 2: {
            if (!item->note.isEmpty())
                return QString("%1 \n* %2").arg(item->name).arg(item->note);
            return item->name;
        }
        case 3:
            return QLocale().toString(item->count);
        case 4:
            return item->unit;
        case 5:
            return Preference::formatMoney(item->price);
        case 6:
            return Preference::formatMoney(item->discount);
        case 7:
            return Preference::formatMoney(item->final);
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case 0:
            return QVariant(Qt::AlignVCenter | Qt::AlignHCenter);
        case 3:
            return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        case 4:
            return QVariant(Qt::AlignVCenter | Qt::AlignHCenter);
        case 5:
        case 6:
        case 7:
            return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        default:
            return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
        }
    }
    return QVariant();
}

bool CashierTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole && mEnableInlineEdit) {
        emit requestEdit(index, value);
    }
    return false;
}

Qt::ItemFlags CashierTableModel::flags(const QModelIndex &index) const {
    // TODO: remove hardcoded here
    if (index.column() == 3 && mEnableInlineEdit)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

QModelIndex CashierTableModel::index(int row, int column, const QModelIndex & /*parent*/) const {
    CashierItem *item = nullptr;
    if (row >= 0 && row < mData.size())
        item = mData[row];
    return createIndex(row, column, item);
}

void CashierTableModel::addItem(float count, const QString &name, const QString &barcode, const QString &unit,
                                const QVariantList &prices, int itemflag, const QString &note) {
    float totCount = getTotalCount(barcode) + count;
    const QList<int> &row = rowOfBarcode(barcode);
    if (!prices.isEmpty())
        mPrices[barcode] = prices;
    if (totCount <= 0 && row.count() > 0) {
        for (int r : row) {
            beginRemoveRows(QModelIndex(), r, r);
            mData.removeAt(r);
            endRemoveRows();
        }
    } else if (totCount > 0) {
        const QList<CashierItem *> &items = calculatePrices(barcode, name, totCount, unit, itemflag, note);
        if (row.count() > 0) {
            if (row.count() < items.count()) {
                // adding more price
                for (int i = 0; i < row.count(); i++) {
                    mData[row[i]]->fill(items[i]);
                    delete items[i];
                }
                emit dataChanged(createIndex(row.first(), 0), createIndex(row.last(), mHeaders.size() - 1));
                beginInsertRows(QModelIndex(), row.last() + 1, row.last() + items.size() - row.size());
                for (int i = row.count(); i < items.count(); i++) {
                    mData.insert(row.last() + i - row.size() + 1, items[i]);
                }
                endInsertRows();
                emit selectRow(createIndex(row.last() + items.size() - row.size(), 0,
                                           mData[row.last() + items.size() - row.size()]));
                emit itemInserted(items.last());
            } else if (row.count() > items.count()) {
                // remove some price
                for (int i = 0; i < items.count(); i++) {
                    mData[row[i]]->fill(items[i]);
                    delete items[i];
                }
                emit dataChanged(createIndex(row.first(), 0), createIndex(row[items.size() - 1], mHeaders.size() - 1));
                for (int i = items.size(); i < row.size(); i++) {
                    beginRemoveRows(QModelIndex(), row[i], row[i]);
                    mData.removeAt(row[i]);
                    endRemoveRows();
                }
                emit selectRow(createIndex(row[items.size() - 1], 0, mData[items.size() - 1]));
                emit itemInserted(items.last());
            } else {
                for (int i = 0; i < items.count(); i++) {
                    mData[row[i]]->fill(items[i]);
                }
                qDeleteAll(items);
                emit dataChanged(createIndex(row.first(), 0), createIndex(row.last(), mHeaders.size() - 1));
                emit selectRow(createIndex(row.last(), 0, mData[row.last()]));
                emit itemInserted(items.last());
            }
        } else {
            beginInsertRows(QModelIndex(), mData.size(), mData.size() + items.size() - 1);
            for (auto item : items) {
                mData.append(item);
            }
            endInsertRows();
            emit selectRow(createIndex(mData.size() - 1, 0, mData[mData.size() - 1]));
            emit itemInserted(items.last());
        }
    }
    calculateTotal();
}

CashierItem *CashierTableModel::addReturnItem(float count, const QString &name, const QString &barcode, double price,
                                              double discount, const QString &unit, int flag) {
    beginInsertRows(QModelIndex(), mData.size() - 1, mData.size() - 1);
    auto item = new CashierItem(name, barcode, -count, price, discount, unit, flag);
    mData.append(item);
    endInsertRows();
    emit selectRow(createIndex(mData.size() - 1, 0, mData[mData.size() - 1]));
    calculateTotal();
    return item;
}

void CashierTableModel::reset() {
    beginResetModel();
    qDeleteAll(mData);
    mData.clear();
    mPrices.clear();
    endResetModel();
    calculateTotal();
    emit totalChanged(0);
}

QVariantList CashierTableModel::getCart() {
    QVariantList list;
    for (auto item : mData) {
        list.append(item->toMap());
    }
    return list;
}

void CashierTableModel::loadCart(const QVariantList &cart) {
    reset();
    beginInsertRows(QModelIndex(), 0, cart.size());
    for (auto c : cart) {
        auto item = new CashierItem();
        item->fill(c.toMap());
        mData.append(item);
    }
    endInsertRows();
    calculateTotal();
}

void CashierTableModel::fillCustomer(const QVariantMap &data) {
    mCurrentCustomer.fill(data);
    // reload poin reward setting
    Message msg(MSG_TYPE::REWARD_POIN, MSG_COMMAND::QUERY);
    msg.setSort("total ASC");
    sendMessage(&msg);
}

CashierItem *CashierTableModel::getItemWithFlag(const QString &barcode, int flag) {
    for (auto item : mData) {
        if (!item->barcode.compare(barcode) && item->hasFlag(flag))
            return item;
    }
    return nullptr;
}

void CashierTableModel::removeReturn(CashierItem *item) {
    int index = mData.indexOf(item);
    beginRemoveRows(QModelIndex(), index, index);
    mData.removeOne(item);
    endRemoveRows();
    calculateTotal();
}

void CashierTableModel::messageReceived(Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::REWARD_POIN, MSG_COMMAND::QUERY)) {
        if (msg->isSuccess()) {
            mRewardPoins.clear();
            const QVariantList &l = msg->data("data").toList();
            for (int i = 0; i < l.size(); i++) {
                const QVariantMap &m = l[i].toMap();
                mRewardPoins.insert(m["total"].toDouble(), m["poin"].toInt());
                calculatePoin();
            }
        }
    }
}

float CashierTableModel::getTotalCount(const QString &barcode) {
    float retVal = 0;
    for (auto item : mData) {
        if (item->hasFlag(CashierItem::Item) && !item->barcode.compare(barcode) && !item->isReturn())
            retVal += item->count;
    }
    return retVal;
}

void CashierTableModel::calculateTotal() {
    mTotal = 0;
    for (auto item : mData)
        mTotal += item->final;
    emit totalChanged(mTotal);
    calculatePoin();
}

QList<int> CashierTableModel::rowOfBarcode(const QString &barcode) {
    QList<int> retVal;
    for (int i = 0; i < mData.size(); i++)
        if (!mData[i]->barcode.compare(barcode))
            retVal << i;
    return retVal;
}

QList<CashierItem *> CashierTableModel::calculatePrices(const QString &barcode, const QString &name, float count,
                                                        const QString &unit, int itemFlag, const QString &note) {
    QList<CashierItem *> retVal;
    bool useMinimum = Preference::getBool(SETTING::MULTIPLE_MINIMUM, false);
    const QVariantList &prices = mPrices[barcode];
    if (useMinimum) {
        for (int i = prices.count() - 1; i >= 0; i--) {
            const QVariantMap &p = prices[i].toMap();
            const float &c = p["count"].toFloat();
            if (i == 0 || c <= count) {
                const double &price = p["price"].toDouble();
                const QString &discformula = p["discount_formula"].toString();
                double disc = Util::calculateDiscount(discformula, price);
                CashierItem *item = new CashierItem(name, barcode, count, price, price * count, discformula, disc,
                                                    (price - disc) * count, unit);
                item->itemFlag = itemFlag;
                item->note = note;
                retVal << item;
                break;
            }
        }
        return retVal;
    }
    if (prices.size() == 1) {
        const double &price = prices[0].toMap()["price"].toDouble();
        const QString &discformula = prices[0].toMap()["discount_formula"].toString();
        double disc = Util::calculateDiscount(discformula, price);
        CashierItem *item = new CashierItem(name, barcode, count, price, price * count, discformula, disc,
                                            (price - disc) * count, unit);
        item->itemFlag = itemFlag;
        item->note = note;
        retVal << item;
    } else {
        for (int i = prices.count() - 1; i >= 0; i--) {
            const QVariantMap &p = prices[i].toMap();
            const double &price = p["price"].toDouble();
            const float &c = p["count"].toFloat();
            const QString &discformula = p["discount_formula"].toString();
            auto item = new CashierItem(name, barcode, 0, price, 0, discformula, 0, 0, unit);
            item->itemFlag = itemFlag;
            item->note = note;
            if (c <= count || i == 0) {
                if (i == 0) {
                    item->total += count * price;
                    item->count = count;
                    item->discount = Util::calculateDiscount(discformula, price);
                    item->final += (count * price) - (count * item->discount);
                    retVal << item;
                } else {
                    float temp = std::fmod(count, c);
                    item->total += (count - temp) * price;
                    item->count = (count - temp);
                    item->discount = Util::calculateDiscount(discformula, price);
                    item->final += ((count - temp) * price) - ((count - temp) * item->discount);
                    count -= (count - temp);
                    retVal << item;
                    if (count <= 0.0f)
                        break;
                }
            }
        }
    }
    return retVal;
}

void CashierTableModel::calculatePoin() {
    mPoin = 0;
    double total = mTotal;
    const QList<double> &p = mRewardPoins.keys();
    for (int i = p.count() - 1; i >= 0; i--) {
        if (p[i] < total) {
            int div = (int)(total / p[i]);
            total = std::fmod(total, p[i]);
            mPoin += (mRewardPoins[p[i]] * div);
        }
    }
    emit poinChanged(mPoin);
}
