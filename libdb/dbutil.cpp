/*
 * dbutil.cpp
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

#include "dbutil.h"
#include <QDateTime>

using namespace LibDB;

DBUtil::DBUtil()
{

}


QString DBUtil::formatDateToIndo(const QString &d, bool useTime)
{
    if(d.isEmpty())
        return d;
    auto date = QDateTime::fromString(d, QLatin1String("yyyy-MM-ddThh:mm:ss"));
    if(useTime)
        return date.toString(QLatin1String("dd-MM-yyyy hh:mm:ss"));
    return date.toString(QLatin1String("dd-MM-yyyy"));
}
