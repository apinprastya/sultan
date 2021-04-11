/*
 * queryhelper.h
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

#ifndef QUERYHELPER_H
#define QUERYHELPER_H

#include "db.h"
#include <functional>

namespace LibDB {

class DBSHARED_EXPORT QueryHelper {
  public:
    QueryHelper();
    static Db *filter(Db *db, const QVariantMap &data,
                      const QMap<QString, QString> &fieldMap = QMap<QString, QString>());
    static Db *sort(Db *db, const QVariantMap &data);
    static Db *limitOffset(Db *db, const QVariantMap &data);
    static void installUserDefinedFilter(int type,
                                         std::function<void(Db *db, const QString &, int, const QVariantMap &)> func);

  private:
    static QString getSign(int type);
};
} // namespace LibDB
#endif // QUERYHELPER_H
