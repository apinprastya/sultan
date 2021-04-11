/*
 * itemutil.cpp
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
#include "itemutil.h"
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "util.h"
#include <QDataStream>
#include <QDebug>
#include <QStringBuilder>
#include <QStringRef>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ItemUtil::ItemUtil(Db *db) : mDb(db) {}

void ItemUtil::updateBuyPrice(const QString &barcode) {
    if (mBarcodeInserted.contains(barcode))
        return;
    mBarcodeInserted << barcode;
    DbResult res = mDb->where("barcode_link = ", barcode)->where("type = ", ITEM_LINK_TYPE::BOX)->get("itemlinks");
    if (!res.isEmpty()) {
        DbResult resItem = mDb->where("barcode = ", barcode)->get("items");
        if (resItem.isEmpty())
            return;
        const QVariantMap &item = resItem.first();
        for (int i = 0; i < res.size(); i++) {
            const QVariantMap &v = res.data(i);
            const QString &b = v["barcode"].toString();
            double p = item["buy_price"].toDouble() * v["count_link"].toFloat();
            mDb->where("barcode = ", b)->update("items", QVariantMap{{"buy_price", p}});
            updateBuyPrice(barcode);
        }
    }
}

void ItemUtil::insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id,
                           bool recursive) {
    insertStock(barcode, number, type, count, flag, link_id, QVariantMap(), recursive);
}

void ItemUtil::insertStock(const QString &barcode, const QString &number, int type, float count, int flag,
                           QVariantMap data, bool recursive) {
    insertStock(barcode, number, type, count, flag, 0, data, recursive);
}

void ItemUtil::insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id,
                           QVariantMap data, bool recursive) {
    if (mBarcodeInserted.contains(barcode))
        return;
    mBarcodeInserted << barcode;
    // first : update is own stock
    DbResult items = mDb->where("barcode = ", barcode)->get("items");
    if (items.isEmpty())
        return;
    const QVariantMap &item = items.first();
    int itemFlag = item["flag"].toInt();
    const QString &name = item["name"].toString();
    float lastStock = 0;
    if ((itemFlag & ITEM_FLAG::CALCULATE_STOCK) != 0) {
        if (type != STOCK_CARD_TYPE::INITIAL_STOCK) {
            DbResult lastsc = mDb->where("barcode = ", barcode)->sort("id DESC")->limit(1)->get("stockcards");
            if (!lastsc.isEmpty())
                lastStock = lastsc.first()["stock"].toFloat();
        }
        mDb->where("barcode = ", barcode)->update("items", QVariantMap{{"stock", item["stock"].toFloat() + count}});
        // insert to stock card
        mDb->insert("stockcards", QVariantMap{{"barcode", barcode},
                                              {"name", name},
                                              {"type", type},
                                              {"number", number},
                                              {"count", count},
                                              {"stock", (lastStock + count)},
                                              {"flag", (itemFlag | flag)},
                                              {"link_id", link_id}});
    }
    if (mLevel == 0) {
        mLevel++;
        if (type == STOCK_CARD_TYPE::SOLD)
            data["buy_price"] = data["count"].toFloat() * item["buy_price"].toDouble();
    }
    // insert to sold if it sold
    if (type == STOCK_CARD_TYPE::SOLD) {
        data["name"] = name;
        data["flag"] = (itemFlag | flag);
        data["unit"] = item["unit"];
        mDb->insert("solditems", data);
        flag |= ITEM_FLAG::ITEM_LINK;
    }
    if (!recursive)
        return;
    // second : update items link stock upper it
    DbResult links = mDb->where("barcode_link = ", barcode)->where("type = ", ITEM_LINK_TYPE::BOX)->get("itemlinks");
    for (int i = 0; i < links.size(); i++) {
        const QVariantMap &link = links.data(i);
        float c = link["count_link"].toFloat();
        data["barcode"] = link["barcode"];
        if (type == STOCK_CARD_TYPE::SOLD) {
            data["count"] = -(count / c);
        }
        insertStock(link["barcode"].toString(), number, type, count / c, flag, link_id, data, recursive);
    }
    // third : update items link stock below it
    links = mDb->where("barcode = ", barcode)->get("itemlinks");
    for (int i = 0; i < links.size(); i++) {
        const QVariantMap &link = links.data(i);
        float c = link["count_link"].toFloat();
        data["barcode"] = link["barcode_link"];
        if (type == STOCK_CARD_TYPE::SOLD) {
            data["count"] = -(count * c);
        }
        insertStock(link["barcode_link"].toString(), number, type, count * c, flag, link_id, data, recursive);
    }
}

void ItemUtil::updateStockCardCount(const QString &barcode, float count, int type, int link_id, bool recursive) {
    if (mBarcodeUpdated.contains(barcode))
        return;
    mBarcodeUpdated.append(barcode);
    DbResult res =
        mDb->where("barcode = ", barcode)->where("type = ", type)->where("link_id = ", link_id)->get("stockcards");
    if (res.isEmpty())
        return;
    const QVariantMap &d = res.first();
    float oldCount = d["count"].toFloat();
    float diff = count - oldCount;
    if (diff == 0)
        return;
    mDb->where("barcode = ", barcode)
        ->where("type = ", type)
        ->where("link_id = ", link_id)
        ->update("stockcards", QVariantMap{{"count", count}});
    mDb->exec(QString("UPDATE stockcards SET stock = stock + %1 WHERE barcode = '%2' AND created_at >= '%3'")
                  .arg(diff)
                  .arg(barcode)
                  .arg(d["created_at"].toString()));
    mDb->exec(QString("UPDATE items SET stock = stock + %1 WHERE barcode = '%2'").arg(diff).arg(barcode));
    if (!recursive)
        return;
    // second : update items link stock upper it
    DbResult links = mDb->where("barcode_link = ", barcode)->where("type = ", ITEM_LINK_TYPE::BOX)->get("itemlinks");
    for (int i = 0; i < links.size(); i++) {
        const QVariantMap &link = links.data(i);
        float c = link["count_link"].toFloat();
        updateStockCardCount(link["barcode"].toString(), count / c, type, link_id, recursive);
    }
    // third : update items link stock below it
    links = mDb->where("barcode = ", barcode)->get("itemlinks");
    for (int i = 0; i < links.size(); i++) {
        const QVariantMap &link = links.data(i);
        float c = link["count_link"].toFloat();
        updateStockCardCount(link["barcode"].toString(), count * c, type, link_id, recursive);
    }
}
