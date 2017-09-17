/*
 * itemaction.cpp
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
#include "itemaction.h"
#include "global_constant.h"
#include "db.h"
#include "queryhelper.h"
#include "util.h"
#include "util/itemutil.h"
#include <QStringRef>
#include <QStringBuilder>
#include <QDataStream>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ItemAction::ItemAction():
    ServerAction("items", "barcode")
{
    mFlag = USE_TRANSACTION;
    mFunctionMap.insert(MSG_COMMAND::CASHIER_PRICE, std::bind(&ItemAction::prices, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&ItemAction::exportData, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::IMPORT, std::bind(&ItemAction::importData, this, std::placeholders::_1));
}

Message ItemAction::insert(Message *msg)
{
    LibG::Message message(msg);
    const int flag = msg->data("flag").toInt();
    const QVariant &sellPrice = msg->takeData("sell_price");
    QVariantMap box = msg->takeData("box").toMap();
    if(mDb->isSupportTransaction()) mDb->beginTransaction();
    if(!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res = mDb->where("barcode = ", mDb->lastInsertedId())->get(mTableName);
        float count = msg->data("stock").toFloat();
        message.setData(res.first());
        if((flag & ITEM_FLAG::MULTIPRICE) == 0) {
            mDb->insert("sellprices", sellPrice.toMap());
        } else {
            const QVariantList &l = sellPrice.toList();
            for(int i = 0; i < l.size(); i++) {
                QVariantMap d = l.at(i).toMap();
                d["barcode"] = msg->data("barcode");
                mDb->insert("sellprices", d);
            }
        }
        if((flag & ITEM_FLAG::PACKAGE) != 0) {
            box.insert("barcode", msg->data("barcode"));
            box.insert("type", ITEM_LINK_TYPE::BOX);
            mDb->insert("itemlinks", box);
            //check stock card to linked item
            DbResult r = mDb->where("barcode = ", box["barcode_link"])->get("items");
            if(!r.isEmpty()) count = r.first()["stock"].toFloat() / box["count_link"].toFloat();
        }
        ItemUtil util(mDb);
        util.insertStock(msg->data("barcode").toString(),
                         QString("Initial %1").arg(msg->data("barcode").toString()),
                         STOCK_CARD_TYPE::INITIAL_STOCK, count, msg->data("flag").toInt(), QVariantMap(), false);
    }
    if(mDb->isSupportTransaction()) {
        if(!mDb->commit()) {
            mDb->roolback();
            message.setError(mDb->lastError().text());
        }
    }
    return message;
}

Message ItemAction::update(Message *msg)
{
    LibG::Message message(msg);
    QVariantMap data = msg->data("data").toMap();
    QVariant sellprice = data["sell_price"];
    QVariantMap box = data.take("box").toMap();
    const int flag = data["flag"].toInt();
    data.remove("sell_price");
    mDb->where(mIdField % " = ", msg->data(mIdField));
    if(!mDb->update(mTableName, data)) {
        message.setError(mDb->lastError().text());
    } else {
        if((flag & ITEM_FLAG::MULTIPRICE) == 0) {
            QVariantMap sp = sellprice.toMap();
            sp["count"] = 1;
            DbResult res = mDb->where("barcode = ", msg->data("barcode"))->get("sellprices");
            if(res.size() == 1) {
                sp.remove("barcode");
                mDb->where("barcode = ", msg->data("barcode"))->update("sellprices", sp);
            } else if(res.size() > 1) {
                mDb->where("barcode = ", msg->data("barcode"))->del("sellprices");
                mDb->insert("sellprices", sp);
            }
            res = mDb->where("id = ", msg->data("id"))->get(mTableName);
            message.setData(res.first());
        }
        if((flag & ITEM_FLAG::PACKAGE) != 0) {
            mDb->where("barcode = ", msg->data("barcode"))->where("type = ", ITEM_LINK_TYPE::BOX);
            mDb->update("itemlinks", box);
        } else {
            mDb->where("barcode = ", msg->data("barcode"))->where("type = ", ITEM_LINK_TYPE::BOX);
            mDb->del("itemlinks");
        }
        ItemUtil util(mDb);
        util.updateBuyPrice(msg->data(mIdField).toString());
    }
    return message;
}

Message ItemAction::del(Message *msg)
{
    LibG::Message message(msg);
    if(hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction())
        mDb->beginTransaction();
    mDb->where(mIdField % " = ", msg->data(mIdField));
    if(!mDb->del(mTableName)) {
        message.setError(mDb->lastError().text());
    } else {
        //remove stok card
        mDb->where("barcode = ", msg->data("barcode"))->del("stockcards");
    }
    if(hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction()) {
        if(!mDb->commit()) {
            mDb->roolback();
            message.setError(mDb->lastError().text());
        }
    }
    return message;
}

LibG::Message ItemAction::prices(LibG::Message *msg)
{
    LibG::Message message(msg);
    const QString &barcode = msg->data("barcode").toString();
    //get item detail
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    mDb->table(mTableName)->where("barcode = ", barcode);
    DbResult res = mDb->exec();
    if(res.isEmpty()) {
        message.setError(QObject::tr("Item not found"));
    } else {
        message.addData("item", res.first());
        mDb->table("sellprices")->where("barcode = ", barcode);
        res = mDb->exec();
        if(res.isEmpty()) {
            message.setError(QObject::tr("Item has no price"));
        } else {
            message.addData("prices", res.data());
        }
    }
    return message;
}

Message ItemAction::exportData(Message *msg)
{
    LibG::Message message(msg);
    QString arr;
    arr.append("barcode;name;category;suplier;stock;buy_price;count1;sellprice1;discform1;count2;sellprice2;discform2;count3;sellprice3;discform3;flag;\n");
    mDb->table(mTableName);
    mDb->select(mTableName % ".*, supliers.name as suplier, categories.name as category, \
                (select count from sellprices where barcode = items.barcode limit 1) as count1, \
                (select price from sellprices where barcode = items.barcode limit 1) as price1, \
                (select discount_formula from sellprices where barcode = items.barcode limit 1) as discform1, \
                (select count from sellprices where barcode = items.barcode limit 1 offset 1) as count2, \
                (select price from sellprices where barcode = items.barcode limit 1 offset 1) as price2, \
                (select discount_formula from sellprices where barcode = items.barcode limit 1 offset 1) as discform2, \
                (select count from sellprices where barcode = items.barcode limit 1 offset 2) as count3, \
                (select price from sellprices where barcode = items.barcode limit 1 offset 2) as price3, \
                (select discount_formula from sellprices where barcode = items.barcode limit 1 offset 2) as discform3")->
            join("LEFT JOIN supliers ON supliers.id = items.suplier_id")->
            join("LEFT JOIN categories ON categories.id = items.category_id");
    const int limit = 500;
    int start = 0;
    while(true) {
        DbResult res = mDb->clone()->start(start)->limit(limit)->exec();
        if(res.isEmpty()) break;
        for(int i = 0; i < res.size(); i++) {
            const QVariantMap &d = res.data(i);
            arr.append(d["barcode"].toString() % ";");
            arr.append(d["name"].toString() % ";");
            arr.append(d["category"].toString() % ";");
            arr.append(d["suplier"].toString() % ";");
            arr.append(d["stock"].toString() % ";");
            arr.append(d["buy_price"].toString() % ";");
            arr.append(d["count1"].toString() % ";");
            arr.append(d["price1"].toString() % ";");
            arr.append(d["discform1"].toString() % ";");
            arr.append(d["count2"].toString() % ";");
            arr.append(d["price2"].toString() % ";");
            arr.append(d["discform2"].toString() % ";");
            arr.append(d["count3"].toString() % ";");
            arr.append(d["price3"].toString() % ";");
            arr.append(d["discform3"].toString() % ";");
            arr.append(d["flag"].toString() % ";\n");
        }
        start += limit;
    }
    message.addData("data", arr);
    return message;
}

Message ItemAction::importData(Message *msg)
{
    LibG::Message message(msg);
    const QString &d = msg->data("data").toString();
    const QVector<QStringRef> &vec = d.splitRef("\n", QString::SkipEmptyParts);
    bool headerOk = false;
    if(mDb->isSupportTransaction()) mDb->beginTransaction();
    for(int i = 0; i < vec.size(); i++) {
        if(!headerOk) {
            headerOk = true;
            continue;
        }
        const QVector<QStringRef> &row = vec[i].split(";", QString::SkipEmptyParts);
        int cat = 0;
        int sup = 0;
        DbResult res = mDb->where("name = ", row[2].toString())->get("categories");
        if(!res.isEmpty()) {
            cat = res.first()["id"].toInt();
        } else {
            QVariantMap catData{{"name", row[2].toString()}, {"code", row[2].toString()}};
            mDb->insert("categories", catData);
            cat = mDb->lastInsertedId().toInt();
        }
        res = mDb->where("name = ", row[3].toString())->get("supliers");
        if(!res.isEmpty()) {
            sup = res.first()["id"].toInt();
        } else {
            QVariantMap supData{{"name", row[3].toString()}, {"code", row[3].toString()}};
            mDb->insert("supliers", supData);
            sup = mDb->lastInsertedId().toInt();
        }
        const QVariantMap ins{{"suplier_id", sup}, {"category_id", cat}, {"barcode", row[0].toString()},
                              {"name", row[1].toString()}, {"stock", row[4].toFloat()},
                              {"buy_price", row[5].toDouble()}, {"flag", row[15].toInt()}};
        if(mDb->insert(mTableName, ins)) {
            for(int i = 0; i < 3; i++) {
                if(row.size() <= (8 + (3 * i))) continue;
                if(row[6 + (3 * i)].isEmpty()) continue;
                bool ok = false;
                float count = row[6 + (3 * i)].toFloat(&ok);
                if(!ok) continue;
                ok = false;
                double price = row[7 + (3 * i)].toDouble(&ok);
                if(!ok) continue;
                const QString &discForm = row[8 + (3 * i)].trimmed().toString();
                double disc = Util::calculateDiscount(discForm, price);
                QVariantMap sellprice{{"barcode", row[0].toString()}, {"count", count},
                                 {"discount_formula", discForm}, {"discount", disc},
                                 {"price", price}, {"final", price - disc}};
                mDb->insert("sellprices", sellprice);
            }
        }
    }
    if(mDb->isSupportTransaction()) {
        if(!mDb->commit()) mDb->roolback();
    }
    return message;
}

void ItemAction::selectAndJoin()
{
    mDb->select(mTableName % ".*, supliers.name as suplier, categories.name as category, \
                (select price from sellprices where barcode = items.barcode order by count asc limit 1) as sell_price, \
                (select count from sellprices where barcode = items.barcode order by count asc limit 1) as sell_count, \
                (select discount_formula from sellprices where barcode = items.barcode order by count asc limit 1) as sell_discount_formula, \
                (select discount from sellprices where barcode = items.barcode order by count asc limit 1) as sell_discount, \
                (select final from sellprices where barcode = items.barcode order by count asc limit 1) as sell_final")->
            join("LEFT JOIN supliers ON supliers.id = items.suplier_id")->
                join("LEFT JOIN categories ON categories.id = items.category_id");
}

QMap<QString, QString> ItemAction::fieldMap() const
{
    QMap<QString, QString> map;
    map.insert("name", "items.name");
    map.insert("suplier", "supliers.name");
    map.insert("category", "items.category_id");
    return map;
}
