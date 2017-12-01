/*
 * purchaseitemaction.cpp
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
#include "purchaseitemaction.h"
#include "db.h"
#include "global_constant.h"
#include "util.h"
#include "util/itemutil.h"
#include <QStringBuilder>
#include <QDateTime>
#include <QDebug>

using namespace LibServer;
using namespace LibDB;
using namespace LibG;

PurchaseItemAction::PurchaseItemAction():
    ServerAction(QStringLiteral("purchaseitems"), QStringLiteral("id"))
{
    mFlag = HAS_UPDATE_FIELD | USE_TRANSACTION;
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&PurchaseItemAction::summary, this, std::placeholders::_1));
}

LibG::Message PurchaseItemAction::insert(LibG::Message *msg)
{
    LibG::Message message(msg);
    double buyPrice = msg->takeData("buy_price").toDouble();
    bool editSellPrice = msg->hasData("sell_price");
    double sellPrice = msg->takeData("sell_price").toDouble();
    double discount = msg->takeData("sell_discount").toDouble();
    float count = msg->takeData("sell_count").toFloat();
    double final = msg->takeData("sell_final").toDouble();
    DbResult res = mDb->where("barcode = ", msg->data("barcode"))->where("purchase_id = ", msg->data("purchase_id"))->get(mTableName);
    if(!res.isEmpty()) {
        message.setError("Item with barcode already on the purchase");
        return message;
    }
    if(hasFlag(HAS_UPDATE_FIELD))
        msg->addData("updated_at", QDateTime::currentDateTime());
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        //update the current stock
        int id = mDb->lastInsertedId().toInt();
        DbResult res = mDb->where("id = ", id)->get(mTableName);
        message.setData(res.first());
        float stock = msg->data("count").toFloat();
        const QString &barcode = msg->data("barcode").toString();
        mDb->exec(QString("UPDATE items SET buy_price = %1 WHERE barcode = '%2'").arg(QString::number(buyPrice)).arg(barcode));
        if(editSellPrice) {
            mDb->exec(QString("UPDATE sellprices SET price = %1, discount = %2, final = %3 WHERE barcode = '%4' AND count = %5").
                      arg(sellPrice).arg(discount).arg(final).arg(barcode).arg(count));
        }
        updatePurchaseTotal(msg->data("purchase_id").toInt());
        DbResult purResult = mDb->where("id = ", msg->data("purchase_id").toInt())->get("purchases");
        ItemUtil util(mDb);
        util.insertStock(barcode, purResult.first()["number"].toString(), STOCK_CARD_TYPE::PURCHASE, stock, 0, id);
    }
    return message;
}

LibG::Message PurchaseItemAction::update(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->where("id = ", msg->data(mIdField));
    QVariantMap d = msg->data("data").toMap();
    double buyPrice = d.take("buy_price").toDouble();
    bool editSellPrice = d.contains("sell_price");
    double sellPrice = d.take("sell_price").toDouble();
    double discount = d.take("sell_discount").toDouble();
    float count = d.take("sell_count").toFloat();
    double final = d.take("sell_final").toDouble();
    if(hasFlag(HAS_UPDATE_FIELD))
        d.insert("updated_at", QDateTime::currentDateTime());
    DbResult res = mDb->clone()->get(mTableName);
    if(!mDb->update(mTableName, d)) {
        message.setError(mDb->lastError().text());
    } else {
        const QVariantMap n = msg->data("data").toMap();
        const QVariantMap old = res.first();
        const int pid = old["purchase_id"].toInt();
        mDb->exec(QString("UPDATE items SET buy_price = %2 WHERE barcode = %3").
                  arg(buyPrice).arg(old["barcode"].toString()));
        if(editSellPrice) {
            mDb->exec(QString("UPDATE sellprices SET price = %1, discount = %2, final = %3 WHERE barcode = %4 AND count = %5").
                      arg(sellPrice).arg(discount).arg(final).arg(old["barcode"].toString()).arg(count));
        }
        updatePurchaseTotal(pid);
        ItemUtil util(mDb);
        util.updateStockCardCount(res.first()["barcode"].toString(), n["count"].toFloat(), STOCK_CARD_TYPE::PURCHASE, msg->data("id").toInt(), true);
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
    ItemUtil util(mDb);
    util.updateStockCardCount(old["barcode"].toString(), 0, STOCK_CARD_TYPE::PURCHASE, msg->data("id").toInt(), true);
    updatePurchaseTotal(old["puchase_id"].toInt());
    return message;
}

LibG::Message PurchaseItemAction::summary(LibG::Message *msg)
{
    LibG::Message message(msg);
    mDb->table("purchaseitems")->where("purchase_id = ", msg->data("purchase_id"));
    DbResult res = mDb->select("SUM(total) as total, SUM(discount) as discount, SUM(final) as final")->exec();
    message.setData(res.first());
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
    DbResult res = mDb->table("purchases")->where("id = ", purchaseid)->exec();
    const QVariantMap &purchase = res.first();
    int ptype = purchase["payment_type"].toInt();
    int status = purchase["status"].toInt();
    const QString &discformula = purchase["discount_formula"].toString();
    res = mDb->table("purchaseitems")->where("purchase_id = ", purchaseid)->select("sum(final) as final")->exec();
    double total = res.first()["final"].toDouble();
    double discount = Util::calculateDiscount(discformula, total);
    mDb->where("id = ", purchaseid)->update("purchases", QVariantMap{{"total", total}, {"discount", discount}, {"final", total - discount}});
    if(ptype == PURCHASEPAYMENT::DIRECT || (ptype == PURCHASEPAYMENT::TEMPO && status == PAYMENT_STATUS::PAID)) {
        mDb->where("link_type = ", TRANSACTION_LINK_TYPE::PURCHASE)->where("link_id = ", purchaseid);
        QVariantMap d{{"transaction_total", -(total - discount)}, {"money_total", -(total - discount)}};
        mDb->update("transactions", d);
    }
}
