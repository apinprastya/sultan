/*
 * permissionhelper.cpp
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
#include "permissionhelper.h"
#include <QStringList>
#include <QStringBuilder>

using namespace LibG;

PermissionHelper::PermissionHelper(const QString &data)
{
    fromString(data);
}

void PermissionHelper::fromString(const QString &data)
{
    const QStringList &l = data.split(",");
    for(auto s : l) {
        const QString &val = s.trimmed();
        if(!val.isEmpty()) {
            mPermissionInt.append(val.toInt());
        }
    }
}

QString PermissionHelper::toString()
{
    QString retVal;
    for(int i = 0; i < mPermissionInt.size(); i++) {
        retVal = retVal % QString::number(mPermissionInt[i]);
        if(i != mPermissionInt.size() - 1)
            retVal = retVal % ",";
    }
    return retVal;
}

bool PermissionHelper::has(int val)
{
    return mPermissionInt.contains(val);
}

void PermissionHelper::add(int val)
{
    mPermissionInt.append(val);
}

void PermissionHelper::rem(int val)
{
    mPermissionInt.removeOne(val);
}
