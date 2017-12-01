/*
 * permissionhelper.h
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
#ifndef PERMISSIONHELPER_H
#define PERMISSIONHELPER_H

#include "global_global.h"
#include <QList>
#include <QString>

namespace LibG {

class GLOBALSHARED_EXPORT PermissionHelper
{
public:
    PermissionHelper() = default;
    PermissionHelper(const QString &data);
    void fromString(const QString &data);
    QString toString();
    bool has(int val);
    void add(int val);
    void rem(int val);

private:
    QList<int> mPermissionInt;
};

}
#endif // PERMISSIONHELPER_H
