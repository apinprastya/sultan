/*
 * barcode.h
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
#ifndef BARCODE_H
#define BARCODE_H

#include "print_global.h"
#include <QList>
#include <QString>

namespace LibPrint {

struct BarcodePrintData {
    BarcodePrintData(const QString &name, const QString &barcode, int price, int copies)
        : name(name), barcode(barcode), price(price), copies(copies) {}
    QString name;
    QString barcode;
    int price;
    int copies;
};

class PRINTSHARED_EXPORT Barcode {
  public:
    Barcode();
    bool print(const QString &name, const QString &barcode, int price, int copies = 1);
    bool print(const QList<BarcodePrintData> &data);
};

} // namespace LibPrint

#endif // BARCODE_H
