/*
 * purchaseitemaction.cpp
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
#include "purchaseitemaction.h"
#include "db.h"
#include <QStringBuilder>

using namespace LibServer;
using namespace LibDB;

PurchaseItemAction::PurchaseItemAction():
    ServerAction(QStringLiteral("purchaseitems"), QStringLiteral("id"))
{
}

LibG::Message PurchaseItemAction::insert(LibG::Message *msg)
{
    LibG::Message message(msg);
    DbResult res = mDb->where("barcode = ", msg->data("barcode"))->where("purchase_id = ", msg->data("purchase_id"))->get(mTableName);
    if(!res.isEmpty()) {
        message.setError("Item with barcode already on the purchase");
        return message;
    }
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        //update the current stock
        float stock = msg->data("count").toFloat();
        const QString &barcode = msg->data("barcode").toString();
        mDb->exec(QString("UPDATE items SET stock = stock + %1 WHERE barcode = %2").arg(QString::number(stock)).arg(barcode));
        updatePurchaseTotal(msg->data("purchase_id").toInt());
        DbResult res = mDb->where("id = ", mDb->lastInsertedId())->get(mTableName);
        message.setData(res.first());
    }
    return message;
}

LibG::Message PurchaseItemAction::update(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->where("id = ", msg->data(mIdField));
    DbResult res = mDb->clone()->get(mTableName);
    if(!mDb->update(mTableName, msg->data("data").toMap())) {
        message.setError(mDb->lastError().text());
    } else {
        const QVariantMap n = msg->data("data").toMap();
        const QVariantMap old = res.first();
        float diff = n["count"].toFloat() - old["count"].toFloat();
        if(n.contains("count") && diff != 0.0f) {
            mDb->exec(QString("UPDATE items SET stock = stock + %1 WHERE barcode = %2").
                      arg(QString::number(diff)).arg(old["barcode"].toString()));
        }
        updatePurchaseTotal(old["puchase_id"].toInt());
        res = mDb->where("id = ", msg->data("id"))->get(mTableName);
        message.setData(res.first());
    }
    return message;
}

LibG::Message PurchaseItemAction::del(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->where("id = ", msg->data(mIdField));
    DbResult res = mDb->clone()->get(mTableName);
    if(!mDb->del(mTableName)) {
        message.setError(mDb->lastError().text());
    }
    const QVariantMap old = res.first();
    mDb->exec(QString("UPDATE items SET stock = stock + %1 WHERE barcode = %2").
              arg(QString::number(-old["count"].toFloat())).arg(old["barcode"].toString()));
    updatePurchaseTotal(old["puchase_id"].toInt());
    return message;
}

QMap<QString, QString> PurchaseItemAction::fieldMap() const
{
    QMap<QString, QString> field;
    field.insert("barcode", "purchaseitems.barcode");
    return field;
}

void PurchaseItemAction::selectAndJoin()
{
    mDb->table(mTableName)->select("purchaseitems.*, items.name, items.buy_price, \
            (select price from sellprices where barcode = purchaseitems.barcode order by count asc limit 1) as sell_price")->
                                   join("LEFT JOIN items ON items.barcode = purchaseitems.barcode");
}

void PurchaseItemAction::updatePurchaseTotal(int purchaseid)
{
    mDb->exec(QString("UPDATE purchases SET total = (SELECT sum(total) FROM purchaseitems WHERE purchase_id = %1) WHERE id = %2").
              arg(purchaseid).arg(purchaseid));
}
