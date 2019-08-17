/*
 * util.h
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
#ifndef UTIL_H
#define UTIL_H

#include "global_global.h"
#include <QDate>

namespace LibG {

class GLOBALSHARED_EXPORT Util
{
public:
    Util();
    static void init(const QString &appDir);
    static QDate getBeginningOfMonth();
    static QDate getEndOfMonth();
    static int getIntVersion(QString version);
    static bool isBetaVersion(QString version);
    static bool isValidDiscountFormula(const QString &val);
    static double calculateDiscount(const QString &formula, double value);
    static QString genSoldNumber();
    static bool hasFlag(int flag, int tocheck);
    static QString appDir();
    static QString capitalize(const QString &str);
    static QString elide(const QString &str, int length);
    static double roundDouble(double val);

private:
};

}
#endif // UTIL_H
