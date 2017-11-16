/*
 * itemutil.h
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
#ifndef ITEMUTIL_H
#define ITEMUTIL_H

#include <QStringList>
#include <QVariantMap>

namespace LibDB {
class Db;
}

namespace LibServer {

/*struct SoldItem {
    SoldItem(){}
    int soldId;
    float count;
    double price;
    double total;
    double discount;
    double final;
    double buyPrice;
    int flag;
    QString discountFormula;
    QString barcode;
    QString name;
};*/

class ItemUtil
{
public:
    ItemUtil(LibDB::Db *db);
    void updateBuyPrice(const QString &barcode);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id, bool recursive = true);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, QVariantMap data, bool recursive = true);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id, QVariantMap data, bool recursive = true);
    void updateStockCardCount(const QString &barcode, float count, int type, int link_id, bool recursive = true);

private:
    LibDB::Db *mDb;
    int mLevel = 0;
    QStringList mBarcodeInserted;
    QStringList mBarcodeUpdated;
};

}

#endif // ITEMUTIL_H
