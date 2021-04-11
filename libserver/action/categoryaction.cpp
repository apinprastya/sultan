/*
 * categoryaction.cpp
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
#include "categoryaction.h"
#include "db.h"
#include <QDateTime>

using namespace LibServer;
using namespace LibDB;

CategoryAction::CategoryAction() : ServerAction("categories", "id") {
    mFlag = AFTER_INSERT | HAS_UPDATE_FIELD | USE_TRANSACTION | SOFT_DELETE | AFTER_DELETE;
}

void CategoryAction::afterInsert(const QVariantMap &data) {
    mDb->insert("categorychilds", QVariantMap{{"category_id", data["id"]}, {"child_id", data["id"]}});
    if (data["parent_id"].toInt() != 0) {
        parentAddChild(data["parent_id"].toInt(), data["id"].toInt());
    }
}

void CategoryAction::afterDelete(const QVariantMap &oldData) {
    mDb->where("parent_id = ", oldData["id"])
        ->update(mTableName, QVariantMap{{"deleted_at", QDateTime::currentDateTime()}});
}

void CategoryAction::parentAddChild(int parent, int child) {
    mDb->insert("categorychilds", QVariantMap{{"category_id", parent}, {"child_id", child}});
    DbResult res = mDb->where("id = ", parent)->get(mTableName);
    if (res.first()["parent_id"].toInt() != 0)
        parentAddChild(res.first()["parent_id"].toInt(), child);
}
