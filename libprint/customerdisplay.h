/*
 * Copyright 2021, Apin <apin.klas@gmail.com>
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

#ifndef CUSTOMERDISPLAY_H
#define CUSTOMERDISPLAY_H

#include "print_global.h"
#include <QByteArray>

namespace LibPrint {

class PRINTSHARED_EXPORT CustomerDisplay {
  public:
    CustomerDisplay();
    CustomerDisplay &clear();
    CustomerDisplay &home();
    CustomerDisplay &bottom();
    CustomerDisplay &left(int step = -1);
    CustomerDisplay &right(int step = -1);
    CustomerDisplay &writeString(const QString &str);
    CustomerDisplay &writeString(const QString &leftStr, const QString &rightStr);
    CustomerDisplay &writeRight(const QString &str);
    QByteArray data();

  private:
    QByteArray mData;
};

} // namespace LibPrint

#endif // CUSTOMERDISPLAY_H
