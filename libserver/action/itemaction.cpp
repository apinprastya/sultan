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
#include "db.h"
#include "global_constant.h"
#include "queryhelper.h"
#include "util.h"
#include "util/configutil.h"
#include "util/itemutil.h"
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QStringBuilder>
#include <QStringRef>

#include "header/xlsxdocument.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ItemAction::ItemAction() : ServerAction("items", "barcode") {
    mFlag = USE_TRANSACTION;
    mFunctionMap.insert(MSG_COMMAND::CASHIER_PRICE, std::bind(&ItemAction::prices, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::EXPORT, std::bind(&ItemAction::exportData, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::IMPORT, std::bind(&ItemAction::importData, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::SUMMARY, std::bind(&ItemAction::summary, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::STOCK, std::bind(&ItemAction::stock, this, std::placeholders::_1));
}

Message ItemAction::insert(Message *msg) {
    LibG::Message message(msg);
    const int flag = msg->data("flag").toInt();
    QVariant sellPrice = msg->takeData("sell_price");
    bool isAutoBarcde = msg->takeData("autobarcode").toBool();
    QVariantMap box = msg->takeData("box").toMap();
    QVariantList ingridients = msg->takeData("ingridients").toList();
    if (mDb->isSupportTransaction())
        mDb->beginTransaction();
    if (isAutoBarcde) {
        int bCounter = ConfigUtil::getIntConfig(mDb, CONFIG_DB::AUTOBARCODE_COUNTER, 0);
        int digit = ConfigUtil::getIntConfig(mDb, CONFIG_DB::AUTOBARCODE_DIGIT, 4);
        if (digit < 3)
            digit = 3;
        const QString &prefix = ConfigUtil::getStringConfig(mDb, CONFIG_DB::AUTOBARCODE_PREFIX, QString());
        DbResult r;
        do {
            bCounter++;
            const QString &barcode = QString("%1%2").arg(prefix).arg(bCounter, digit, 10, QChar('0'));
            r = mDb->where("barcode = ", barcode)->get("items");
            msg->addData("barcode", barcode);
        } while (!r.isEmpty());
        ConfigUtil::setConfig(mDb, CONFIG_DB::AUTOBARCODE_COUNTER, bCounter);
        QVariantMap sp = sellPrice.toMap();
        sp["barcode"] = msg->data("barcode");
        sellPrice = sp;
    }
    float count = msg->data("stock").toFloat();
    msg->addData("stock", 0);
    if (!mDb->insert(mTableName, msg->data())) {
        message.setError(mDb->lastError().text());
    } else {
        DbResult res =
            mDb->where("barcode = ", mDb->isSQLite() ? msg->data("barcode") : mDb->lastInsertedId())->get(mTableName);
        message.setData(res.first());
        if ((flag & ITEM_FLAG::MULTIPRICE) == 0) {
            mDb->insert("sellprices", sellPrice.toMap());
        } else {
            const QVariantList &l = sellPrice.toList();
            for (int i = 0; i < l.size(); i++) {
                QVariantMap d = l.at(i).toMap();
                d["barcode"] = msg->data("barcode");
                mDb->insert("sellprices", d);
            }
        }
        if ((flag & ITEM_FLAG::PACKAGE) != 0) {
            box.insert("barcode", msg->data("barcode"));
            box.insert("type", ITEM_LINK_TYPE::BOX);
            mDb->insert("itemlinks", box);
            // check stock card to linked item
            DbResult r = mDb->where("barcode = ", box["barcode_link"])->get("items");
            if (!r.isEmpty())
                count = r.first()["stock"].toFloat() / box["count_link"].toFloat();
        }
        ItemUtil util(mDb);
        util.insertStock(msg->data("barcode").toString(), QString("Initial %1").arg(msg->data("barcode").toString()),
                         STOCK_CARD_TYPE::INITIAL_STOCK, count, msg->data("flag").toInt(), QVariantMap(), false);
        if ((flag & ITEM_FLAG::HAS_INGRIDIENT) != 0) {
            for (int i = 0; i < ingridients.size(); i++) {
                const QVariantMap &d = ingridients[i].toMap();
                mDb->insert("itemlinks", QVariantMap{{"barcode", msg->data("barcode")},
                                                     {"type", ITEM_LINK_TYPE::INGRIDIENT},
                                                     {"barcode_link", d["barcode_link"]},
                                                     {"count_link", d["count_link"]}});
            }
        }
    }
    if (mDb->isSupportTransaction()) {
        if (!mDb->commit()) {
            mDb->roolback();
            message.setError(mDb->lastError().text());
        }
    }
    return message;
}

Message ItemAction::update(Message *msg) {
    LibG::Message message(msg);
    QVariantMap data = msg->data("data").toMap();
    QVariant sellprice = data["sell_price"];
    QVariantMap box = data.take("box").toMap();
    const int flag = data["flag"].toInt();
    data.remove("sell_price");
    mDb->where(mIdField % " = ", msg->data(mIdField));
    if (!mDb->update(mTableName, data)) {
        message.setError(mDb->lastError().text());
    } else {
        if ((flag & ITEM_FLAG::MULTIPRICE) == 0) {
            QVariantMap sp = sellprice.toMap();
            sp["count"] = 1;
            DbResult res = mDb->where("barcode = ", msg->data("barcode"))->get("sellprices");
            if (res.size() == 1) {
                sp.remove("barcode");
                mDb->where("barcode = ", msg->data("barcode"))->update("sellprices", sp);
            } else if (res.size() > 1) {
                mDb->where("barcode = ", msg->data("barcode"))->del("sellprices");
                mDb->insert("sellprices", sp);
            } else if (res.size() == 0) {
                mDb->insert("sellprices", sp);
            }
            res = mDb->where("id = ", msg->data("id"))->get(mTableName);
            message.setData(res.first());
        }
        if ((flag & ITEM_FLAG::PACKAGE) != 0) {
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

Message ItemAction::del(Message *msg) {
    LibG::Message message(msg);
    if (hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction())
        mDb->beginTransaction();
    QVariantList ids;
    if (msg->data(mIdField).canConvert<QString>())
        ids.append(msg->data(mIdField));
    else
        ids = msg->data(mIdField).toList();
    for (int i = 0; i < ids.size(); i++) {
        mDb->where(mIdField % " = ", ids[i]);
        if (hasFlag(SOFT_DELETE)) {
            if (!mDb->update(mTableName, QVariantMap{{"deleted_at", QDateTime::currentDateTime()}, {"stock", 0}})) {
                message.setError(mDb->lastError().text());
            }
        } else {
            if (!mDb->del(mTableName)) {
                message.setError(mDb->lastError().text());
            }
        }
        mDb->where("barcode = ", ids[i])->del("stockcards");
        mDb->where("barcode = ", ids[i])->del("itemlinks");
        mDb->where("barcode_link = ", ids[i])->del("itemlinks");
    }

    if (hasFlag(USE_TRANSACTION) && mDb->isSupportTransaction()) {
        if (!mDb->commit()) {
            mDb->roolback();
            message.setError(mDb->lastError().text());
        }
    }
    return message;
}

LibG::Message ItemAction::prices(LibG::Message *msg) {
    LibG::Message message(msg);
    const QString &barcode = msg->data("barcode").toString();
    // get item detail
    if (hasFlag(SOFT_DELETE) && !msg->hasData("withdeleted"))
        mDb->where("deleted_at IS NULL");
    mDb = QueryHelper::filter(mDb, msg->data(), fieldMap());
    mDb->table(mTableName)->where("barcode = ", barcode);
    DbResult res = mDb->exec();
    if (res.isEmpty()) {
        message.setError(QObject::tr("Item not found"));
    } else {
        message.addData("item", res.first());
        mDb->table("sellprices")->where("barcode = ", barcode);
        res = mDb->exec();
        if (res.isEmpty()) {
            message.setError(QObject::tr("Item has no price"), false);
        } else {
            message.addData("prices", res.data());
        }
    }
    return message;
}

Message ItemAction::exportData(Message *msg) {
    LibG::Message message(msg);

    QXlsx::Document xlsx;
    int row = 1;
    int col = 1;
    xlsx.write(row, 1, msg->data("version").toString());

    DbResult cats = mDb->table("categories")->where("deleted_at IS NULL")->exec();
    xlsx.write(row++, 1, "###CATEGORY");
    xlsx.write(row, 1, "id");
    xlsx.write(row, 2, "name");
    xlsx.write(row, 3, "code");
    xlsx.write(row, 4, "parent_id");
    xlsx.write(row, 5, "hierarchy");
    row++;
    for (int i = 0; i < cats.size(); i++) {
        const QVariantMap &d = cats.data(i);
        xlsx.write(row, 1, d["id"]);
        xlsx.write(row, 2, d["name"]);
        xlsx.write(row, 3, d["code"]);
        xlsx.write(row, 4, d["parent_id"]);
        xlsx.write(row, 5, d["hierarchy"]);
        row++;
    }

    DbResult sups = mDb->table("supliers")->where("deleted_at IS NULL")->exec();
    xlsx.write(row++, 1, "###SUPPLIER");
    xlsx.write(row, 1, "id");
    xlsx.write(row, 2, "name");
    xlsx.write(row, 3, "code");
    xlsx.write(row, 4, "address");
    xlsx.write(row, 5, "phone");
    xlsx.write(row, 6, "email");
    row++;
    for (int i = 0; i < sups.size(); i++) {
        const QVariantMap &d = sups.data(i);
        xlsx.write(row, 1, d["id"]);
        xlsx.write(row, 2, d["name"]);
        xlsx.write(row, 3, d["code"]);
        xlsx.write(row, 4, d["address"]);
        xlsx.write(row, 5, d["phone"]);
        xlsx.write(row, 6, d["email"]);
        row++;
    }

    xlsx.write(row++, 1, "###ITEM");
    auto headers =
        QString("barcode;name;category;suplier;stock;buy_price;count1;sellprice1;discform1;count2;sellprice2;discform2;"
                "count3;sellprice3;discform3;calculatestock;sellable;purchaseable;box;multiprice;priceeditable;unit")
            .split(";");
    for (auto header : headers) {
        xlsx.write(row, col++, header);
    }
    row++;
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
                (select discount_formula from sellprices where barcode = items.barcode limit 1 offset 2) as discform3")
        ->join("LEFT JOIN supliers ON supliers.id = items.suplier_id")
        ->join("LEFT JOIN categories ON categories.id = items.category_id");
    const int limit = 500;
    int start = 0;
    while (true) {
        DbResult res = mDb->clone()->start(start)->limit(limit)->exec();
        if (res.isEmpty())
            break;
        for (int i = 0; i < res.size(); i++) {
            const QVariantMap &d = res.data(i);
            int flag = d["flag"].toInt();
            xlsx.write(row, 1, d["barcode"].toString().trimmed().replace("\n", ""));
            xlsx.write(row, 2, d["name"].toString().trimmed().replace("\n", ""));
            xlsx.write(row, 3, d["category"].toString().trimmed().replace("\n", ""));
            xlsx.write(row, 4, d["suplier"].toString().trimmed().replace("\n", ""));
            xlsx.write(row, 5, d["stock"]);
            xlsx.write(row, 6, d["buy_price"]);
            xlsx.write(row, 7, d["count1"]);
            xlsx.write(row, 8, d["price1"]);
            xlsx.write(row, 9, d["discform1"]);
            xlsx.write(row, 10, d["count2"]);
            xlsx.write(row, 11, d["price2"]);
            xlsx.write(row, 12, d["discform2"]);
            xlsx.write(row, 13, d["count3"]);
            xlsx.write(row, 14, d["price3"]);
            xlsx.write(row, 15, d["discform3"]);
            xlsx.write(row, 16, (flag & ITEM_FLAG::CALCULATE_STOCK) == 0 ? "0" : "1");
            xlsx.write(row, 17, (flag & ITEM_FLAG::SELLABLE) == 0 ? "0" : "1");
            xlsx.write(row, 18, (flag & ITEM_FLAG::PURCHASE) == 0 ? "0" : "1");
            xlsx.write(row, 19, (flag & ITEM_FLAG::PACKAGE) == 0 ? "0" : "1");
            xlsx.write(row, 20, (flag & ITEM_FLAG::MULTIPRICE) == 0 ? "0" : "1");
            xlsx.write(row, 21, (flag & ITEM_FLAG::EDITABLE_PRICE) == 0 ? "0" : "1");
            xlsx.write(row, 22, d["unit"].toString().trimmed());
            row++;
        }
        start += limit;
    }

    xlsx.write(row++, 1, "###LINK");
    xlsx.write(row, 1, "barcode");
    xlsx.write(row, 2, "type");
    xlsx.write(row, 3, "barcode_link");
    xlsx.write(row, 4, "count_link");
    row++;
    DbResult linkres = mDb->reset()->get("itemlinks");
    for (int i = 0; i < linkres.size(); i++) {
        const QVariantMap &d = linkres.data(i);
        xlsx.write(row, 1, d["barcode"].toString().trimmed());
        xlsx.write(row, 2, d["type"]);
        xlsx.write(row, 3, d["barcode_link"].toString().trimmed());
        xlsx.write(row, 4, d["count_link"]);
        row++;
    }

    QBuffer arr;
    xlsx.saveAs(&arr);
    message.addData("data", QString(arr.data().toBase64()));

    return message;
}

Message ItemAction::importData(Message *msg) {
    LibG::Message message(msg);
    const QString &d = msg->data("data").toString();
    const QVector<QStringRef> &vec = d.splitRef("\n", QString::SkipEmptyParts);
    int state = 0; // 0: category, 1: item, 2: link
    int version = 0;
    if (mDb->isSupportTransaction())
        mDb->beginTransaction();
    for (int i = 0; i < vec.size(); i++) {
        QString str = vec[i].toString().replace("\"", "");
        QStringRef sRef(&str);
        if (i == 0) {
            if (sRef.startsWith("VERSION")) {
                const QVector<QStringRef> sp = sRef.split(" ");
                if (sp.size() > 1)
                    version = sp[1].toString().replace(".", "").replace(";", "").toInt();
                mDb->truncateTable("categories");
                mDb->truncateTable("supliers");
                mDb->truncateTable("items");
                mDb->truncateTable("sellprices");
                mDb->truncateTable("itemlinks");
                continue;
            } else {
                state = 1;
            }
        }
        if (sRef.startsWith(QStringLiteral("###CATEGORY"))) {
            state = 0;
            i++;
            continue;
        } else if (sRef.startsWith(QStringLiteral("###ITEM"))) {
            state = 1;
            i++;
            continue;
        } else if (sRef.startsWith(QStringLiteral("###LINK"))) {
            state = 2;
            i++;
            continue;
        } else if (sRef.startsWith(QStringLiteral("###SUPPLIER"))) {
            state = 3;
            i++;
            continue;
        }
        const QVector<QStringRef> &row = sRef.split(";");
        if (state == 0) {
            const QString &name = row[1].toString();
            const QString &code = row[2].toString();
            const QString &hierarcy = row[4].toString();
            const int parentId = row[3].toInt();
            mDb->insert("categories",
                        QVariantMap{{"name", name}, {"code", code}, {"hierarchy", hierarcy}, {"parent_id", parentId}});
        } else if (state == 1) {
            int cat = 0;
            int sup = 0;
            int flag = 0;
            if ((row[15].toInt() != 0))
                flag |= ITEM_FLAG::CALCULATE_STOCK;
            if ((row[16].toInt() != 0))
                flag |= ITEM_FLAG::SELLABLE;
            if ((row[17].toInt() != 0))
                flag |= ITEM_FLAG::PURCHASE;
            if ((row[18].toInt() != 0))
                flag |= ITEM_FLAG::PACKAGE;
            if ((row[19].toInt() != 0))
                flag |= ITEM_FLAG::MULTIPRICE;
            if ((row[20].toInt() != 0))
                flag |= ITEM_FLAG::EDITABLE_PRICE;
            DbResult res = mDb->where("name = ", row[2].toString())->get("categories");
            if (!res.isEmpty()) {
                cat = res.first()["id"].toInt();
            } else {
                QVariantMap catData{{"name", row[2].toString()}, {"code", row[2].toString()}};
                mDb->insert("categories", catData);
                cat = mDb->lastInsertedId().toInt();
            }
            res = mDb->where("name = ", row[3].toString())->get("supliers");
            if (!res.isEmpty()) {
                sup = res.first()["id"].toInt();
            } else {
                QVariantMap supData{{"name", row[3].toString()}, {"code", row[3].toString()}};
                mDb->insert("supliers", supData);
                sup = mDb->lastInsertedId().toInt();
            }
            const QVariantMap ins{{"suplier_id", sup},
                                  {"category_id", cat},
                                  {"barcode", row[0].toString()},
                                  {"name", row[1].toString()},
                                  {"stock", row[4].toFloat()},
                                  {"buy_price", row[5].toDouble()},
                                  {"flag", flag},
                                  {"unit", row[21].toString()}};
            if (mDb->insert(mTableName, ins)) {
                for (int i = 0; i < 3; i++) {
                    if (row.size() <= (8 + (3 * i)))
                        continue;
                    if (row[6 + (3 * i)].isEmpty())
                        continue;
                    bool ok = false;
                    float count = row[6 + (3 * i)].toFloat(&ok);
                    if (!ok)
                        continue;
                    ok = false;
                    double price = row[7 + (3 * i)].toDouble(&ok);
                    if (!ok)
                        continue;
                    const QString &discForm = row[8 + (3 * i)].trimmed().toString();
                    double disc = Util::calculateDiscount(discForm, price);
                    QVariantMap sellprice{{"barcode", row[0].toString()},
                                          {"count", count},
                                          {"discount_formula", discForm},
                                          {"discount", disc},
                                          {"price", price},
                                          {"final", price - disc}};
                    mDb->insert("sellprices", sellprice);
                }
            }
        } else if (state == 2) {
            const QString &barcode = row[0].toString();
            const QString &barcode_link = row[2].toString();
            const QString &count_link = row[3].toString();
            const int type = row[1].toString().toInt();
            DbResult res = mDb->where("barcode =", barcode)
                               ->where("barcode_link =", barcode_link)
                               ->where("type =", type)
                               ->get("itemlinks");
            if (res.isEmpty()) {
                mDb->insert("itemlinks", QVariantMap{{"barcode", barcode},
                                                     {"barcode_link", barcode_link},
                                                     {"type", type},
                                                     {"count_link", count_link}});
            }
        } else if (state == 3) {
            const QString &name = row[1].toString();
            const QString &code = row[2].toString();
            const QString &address = row[3].toString();
            const QString &phone = row[4].toString();
            const QString &email = row[5].toString();
            mDb->insert(
                "supliers",
                QVariantMap{{"name", name}, {"code", code}, {"address", address}, {"phone", phone}, {"email", email}});
        }
    }
    if (mDb->isSupportTransaction()) {
        if (!mDb->commit())
            mDb->roolback();
    }
    return message;
}

Message ItemAction::summary(Message *msg) {
    Message message(msg);
    DbResult res = mDb->select("sum(stock * buy_price) as total")
                       ->where("stock > 0")
                       ->where(QString("(flag & %1) = 0").arg(ITEM_FLAG::PACKAGE))
                       ->where(QString("(flag & %1) = 0").arg(ITEM_FLAG::HAS_INGRIDIENT))
                       ->get(mTableName);
    if (!res.isEmpty())
        message.addData("total", res.first()["total"]);
    else
        message.addData("total", 0);
    return message;
}

Message ItemAction::stock(LibG::Message *msg) {
    Message message(msg);
    auto barcodeVariants = msg->data("barcodes").toList();
    QStringList barcodes;
    for (auto v : barcodeVariants)
        barcodes << v.toString();
    DbResult res = mDb->table(mTableName)
                       ->select("barcode")
                       ->select("stock")
                       ->where(QString("barcode in (%1)").arg(barcodes.join(",")))
                       ->exec();
    message.addData("data", res.data());
    return message;
}

void ItemAction::selectAndJoin() {
    mDb->select(mTableName % ".*, supliers.name as suplier, categories.name as category, \
                (select price from sellprices where barcode = items.barcode order by count asc limit 1) as sell_price, \
                (select count from sellprices where barcode = items.barcode order by count asc limit 1) as sell_count, \
                (select discount_formula from sellprices where barcode = items.barcode order by count asc limit 1) as sell_discount_formula, \
                (select discount from sellprices where barcode = items.barcode order by count asc limit 1) as sell_discount, \
                (select final from sellprices where barcode = items.barcode order by count asc limit 1) as sell_final")
        ->join("LEFT JOIN supliers ON supliers.id = items.suplier_id")
        ->join("LEFT JOIN categories ON categories.id = items.category_id");
}

QMap<QString, QString> ItemAction::fieldMap() const {
    QMap<QString, QString> map;
    map.insert("name", "items.name");
    map.insert("suplier", "supliers.name");
    map.insert("category", "items.category_id");
    return map;
}
