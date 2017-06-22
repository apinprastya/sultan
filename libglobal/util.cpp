/*
 * util.cpp
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
#include "util.h"

using namespace LibG;

Util::Util()
{

}

QDate Util::getBeginningOfMonth()
{
    auto now = QDate::currentDate();
    QDate retVal;
    retVal.setDate(now.year(), now.month(), 1);
    return retVal;
}

QDate Util::getEndOfMonth()
{
    auto retVal = getBeginningOfMonth();
    retVal = retVal.addMonths(1);
    retVal = retVal.addDays(-1);
    return retVal;
}

int Util::getIntVersion(QString version)
{
    version = version.replace(".", "");
    return version.toInt();
}

bool Util::isBetaVersion(QString version)
{
    return version.length() > 8;
}

bool Util::isValidDiscountFormula(const QString &val)
{
    if(val.isEmpty()) return true;
    const QStringList &l = val.split("+");
    for(const QString &val : l) {
        if(val.endsWith("%")) {
            bool ok = false;
            val.mid(0, val.size() - 1).toDouble(&ok);
            if(!ok) return false;
        } else {
            bool ok = false;
            val.toDouble(&ok);
            if(!ok) return false;
        }
    }
    return true;
}

double Util::calculateDiscount(const QString &formula, double value)
{
    if(formula.isEmpty()) return 0;
    double retVal = 0;
    const QStringList &l = formula.split("+");
    for(const QString &val : l) {
        if(val.endsWith("%")) {
            bool ok = false;
            double disc = val.mid(0, val.size() - 1).toDouble(&ok);
            if(ok) {
                retVal += value * disc / 100;
            }
        } else {
            bool ok = false;
            double disc = val.toDouble(&ok);
            if(ok) {
                retVal += disc;
            }
        }
        value -= retVal;
    }
    return retVal;
}
