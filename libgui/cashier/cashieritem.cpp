/*
 * cashieritem.cpp
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
#include "cashieritem.h"

using namespace LibGUI;

static int ID = 1;

CashierItem::CashierItem():
    id(ID++)
{
}

CashierItem::CashierItem(const QString &name, const QString &barcode, float count, double price, double total,const QString &discformula, double discount, double final, int type):
    id(ID++)
{
    set(name, barcode, count, price, total, discformula, discount, final, type);
}

void CashierItem::set(const QString &name, const QString &barcode, float count, double price, double total, const QString &discformula, double discount, double final, int type)
{
    this->name = name;
    this->barcode = barcode;
    this->count = count;
    this->price = price;
    this->total = total;
    this->type = type;
    this->discount_formula = discformula;
    this->discount = discount;
    this->final = final;
}

void CashierItem::fill(const QVariantMap &data)
{
    id = data["id"].toInt();
    count = data["count"].toFloat();
    price = data["price"].toDouble();
    total = data["total"].toDouble();
    barcode = data["barcode"].toString();
    discount = data["discount"].toDouble();
    discount_formula = data["discount_formula"].toString();
    final = data["final"].toDouble();
    name = data["name"].toString();
}

QVariantMap CashierItem::toMap()
{
    QVariantMap data;
    data.insert("barcode", barcode);
    data.insert("name", name);
    data.insert("count", count);
    data.insert("price", price);
    data.insert("total", total);
    data.insert("discount", discount);
    data.insert("discount_formula", discount_formula);
    data.insert("final", final);
    return data;
}

CashierItem *CashierItem::clone()
{
    auto item = new CashierItem(name, barcode, count, price, total, discount_formula, discount, final, type);
    item->id = id;
    item->parent = parent;
    return item;
}

void CashierItem::fill(CashierItem *another)
{
    id = another->id;
    parent = another->parent;
    type = another->type;
    count = another->count;
    price = another->price;
    total = another->total;
    barcode = another->barcode;
    name = another->name;
    discount_formula = another->discount_formula;
    discount = another->discount;
    final = another->final;
}
