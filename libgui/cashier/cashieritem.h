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
    enum Flag {
        Item        = 0x1,
        Service     = 0x2,
        Return      = 1 << 8,
        Returned    = 1 << 9
    };
    int id = 0;
    int parent = 0;
    int flag = Item;
    float count = 0;
    double price = 0;
    double discount = 0;
    double total = 0;
    double final = 0;
    int linkId = 0;
    double buyPrice;
    int itemFlag = 0;
    QString barcode;
    QString name;
    QString discount_formula;
    QString unit;
    QString note;
    CashierItem();
    CashierItem(const QString &name, const QString &barcode, float count, double price, double total, const QString &discformula, double discount, double final, const QString &unit, int flag = Item);
    CashierItem(const QString &name, const QString &barcode, float count, double price, double discount,  const QString &unit, int flag);
    void set(const QString &name, const QString &barcode, float count, double price, double total, const QString &discformula, double discount, double final, const QString &unit, int flag = Item);
    void fill(const QVariantMap &data);
    QVariantMap toMap();
    CashierItem *clone();
    void fill(CashierItem *another);
    inline bool hasFlag(int flag) { return (this->flag & flag) != 0; }
    inline bool isReturn() { return hasFlag(Return); }
    inline bool isReturned() { return hasFlag(Returned); }
    inline void setNote(const QString &note) { this->note = note; }
};

}
#endif // CASHIERITEM_H
