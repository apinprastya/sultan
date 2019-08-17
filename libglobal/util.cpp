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
#include "preference.h"
#include "global_constant.h"
#include "global_setting_const.h"

using namespace LibG;

static int NEXTVAL = 0;
static QString sAppDir;

Util::Util()
{

}

void Util::init(const QString &appDir)
{
    sAppDir = appDir;
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
        double locDisc = 0;
        if(val.endsWith("%")) {
            bool ok = false;
            double disc = val.mid(0, val.size() - 1).toDouble(&ok);
            if(ok) {
                locDisc = value * disc / 100;
                retVal += locDisc;
            }
        } else {
            bool ok = false;
            locDisc = val.toDouble(&ok);
            if(ok) {
                retVal += locDisc;
            }
        }
        value -= locDisc;
    }
    return retVal;
}

QString Util::genSoldNumber()
{
    QString name = Preference::getString(SETTING::MACHINE_CODE);
    name.replace(QChar(' '), QChar('_'));
    const QString &now = QDate::currentDate().toString("yyMMdd");
    if(NEXTVAL == 0) {
        const QString &old = Preference::getString(SETTING::NUMBER_DATE);
        if(now != old) {
            NEXTVAL = 1;
            Preference::setValue(SETTING::NUMBER_DATE, now);
        } else {
            NEXTVAL = Preference::getInt(SETTING::NUMBER_VALUE);
        }
    }
    int val = NEXTVAL;
    NEXTVAL++;
    Preference::setValue(SETTING::NUMBER_VALUE, NEXTVAL);
    const QString &ret = QString("%1-%2%3").arg(name).arg(now).arg(val, 3, 16, QChar('0'));
    return ret;
}

bool Util::hasFlag(int flag, int tocheck)
{
    return (flag & tocheck) != 0;
}

QString Util::appDir()
{
    return sAppDir;
}

QString Util::capitalize(const QString &str)
{
    bool cap = Preference::getBool(SETTING::CAPITALIZE);
    return cap ? str.toUpper() : str;
}

QString Util::elide(const QString &str, int length)
{
    if(length <= 5 || str.length() <= length) return str;
    bool isOdd = ((length % 2) != 0);
    int l = (length / 2) - 1;
    return QString("%1..%2").arg(str.left(isOdd ? l + 1 : l)).arg(str.right(l));
}

double Util::roundDouble(double val)
{
    qint64 v = (qint64)(val * 1000);
    return ((double)v) / 1000;
}
