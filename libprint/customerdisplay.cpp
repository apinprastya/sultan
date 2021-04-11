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

#include "customerdisplay.h"
#include <QString>

using namespace LibPrint;

CustomerDisplay::CustomerDisplay() {}

CustomerDisplay &CustomerDisplay::clear() {
    mData.append((char)0x0c);
    return *this;
}

CustomerDisplay &CustomerDisplay::home() {
    mData.append((char)0x0b);
    return *this;
}

CustomerDisplay &CustomerDisplay::bottom() {
    mData.append((char)0x0a);
    return *this;
}

CustomerDisplay &CustomerDisplay::left(int step) {
    if (step < 0) {
        mData.append((char)0x0d);
    } else {
        for (int i = 0; i < step; i++)
            mData.append((char)0x08);
    }
    return *this;
}

CustomerDisplay &CustomerDisplay::right(int step) {
    if (step < 0) {
        mData.append((char)0x1f).append((char)0x0d);
    } else {
        for (int i = 0; i < step; i++)
            mData.append((char)0x09);
    }
    return *this;
}

CustomerDisplay &CustomerDisplay::writeString(const QString &str) {
    mData.append(str.toUtf8());
    return *this;
}

CustomerDisplay &CustomerDisplay::writeString(const QString &leftStr, const QString &rightStr) {
    left();
    mData.append(leftStr.toUtf8());
    right();
    left(rightStr.toUtf8().length());
    mData.append(rightStr.toUtf8());
    return *this;
}

CustomerDisplay &CustomerDisplay::writeRight(const QString &str) {
    right();
    left(str.toUtf8().length());
    mData.append(str.toUtf8());
    return *this;
}

QByteArray CustomerDisplay::data() { return mData; }
