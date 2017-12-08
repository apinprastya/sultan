/*
 * dbresult.h
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

#ifndef DBRESULT_H
#define DBRESULT_H

#include "db_global.h"

#include <QList>
#include <QVariantMap>

namespace LibDB {

class DBSHARED_EXPORT DbResult
{
public:
    DbResult();
    DbResult(const QList<QVariant> &data);
    inline bool isEmpty() { return mData.isEmpty(); }
    QVariantMap first();
    inline int size() { return mData.size(); }
    QVariantMap data(int pos);
    inline QVariantList &data() { return mData; }

private:
    QVariantList mData;
};

}
#endif // DBRESULT_H
