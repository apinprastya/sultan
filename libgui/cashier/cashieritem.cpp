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

CashierItem::CashierItem(const QString &name, const QString &barcode, float count, double price, double total,const QString &discformula, double discount, double final, const QString &unit, int type):
    id(ID++)
{
    set(name, barcode, count, price, total, discformula, discount, final, unit, type);
}

CashierItem::CashierItem(const QString &name, const QString &barcode, float count, double price, double discount, const QString &unit, int flag)
{
    set(name, barcode, count, price, price - discount, "", discount, (price - discount) * count, unit, flag);
}

void CashierItem::set(const QString &name, const QString &barcode, float count, double price, double total, const QString &discformula, double discount, double final, const QString &unit, int type)
{
    this->name = name;
    this->barcode = barcode;
    this->count = count;
    this->price = price;
    this->total = total;
    this->flag = type;
    this->discount_formula = discformula;
    this->discount = discount;
    this->final = final;
    this->unit = unit;
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
    flag = data["flag"].toInt();
    linkId = data["link_id"].toInt();
    buyPrice = data["buy_price"].toDouble();
    unit = data["unit"].toString();
    note = data["note"].toString();
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
    data.insert("flag", flag);
    data.insert("link_id", linkId);
    data.insert("buy_price", buyPrice);
    data.insert("unit", unit);
    data.insert("note", note);
    return data;
}

CashierItem *CashierItem::clone()
{
    auto item = new CashierItem(name, barcode, count, price, total, discount_formula, discount, final, unit, flag);
    item->id = id;
    item->parent = parent;
    item->flag = flag;
    item->linkId = linkId;
    item->buyPrice = buyPrice;
    return item;
}

void CashierItem::fill(CashierItem *another)
{
    id = another->id;
    parent = another->parent;
    flag = another->flag;
    count = another->count;
    price = another->price;
    total = another->total;
    barcode = another->barcode;
    name = another->name;
    discount_formula = another->discount_formula;
    discount = another->discount;
    final = another->final;
    flag = another->flag;
    linkId = another->linkId;
    buyPrice = another->buyPrice;
    unit = another->unit;
    if(!another->note.isNull()) note = another->note;
}
