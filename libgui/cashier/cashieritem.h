/*
 * cashieritem.h
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
#ifndef CASHIERITEM_H
#define CASHIERITEM_H

#include <QVariantMap>
#include <QString>

namespace LibGUI {

class CashierItem
{
public:
    enum Type { Item, Discount };
    int id = 0;
    int parent = 0;
    int type = Item;
    float count = 0;
    double price = 0;
    double total = 0;
    QString barcode;
    QString name;
    CashierItem();
    CashierItem(const QString &name, const QString &barcode, float count, double price, double total, int type = Item);
    void set(const QString &name, const QString &barcode, float count, double price, double total, int type = Item);
    void fill(const QVariantMap &data);
};

}
#endif // CASHIERITEM_H
