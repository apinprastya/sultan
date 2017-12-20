/*
 * printer.h
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
#ifndef PRINTER_H
#define PRINTER_H

#include "print_global.h"
#include <QStringList>
#include <QMutex>

namespace LibPrint {

class PRINTSHARED_EXPORT Printer
{
private:
    Printer();
    QMutex mMutex;

public :
    static Printer *instance();
    static void destroy();
    QStringList getPrintList();
    void print(const QString &printName, const QString &data, int type = 1, uint16_t vendorId = 0, uint16_t produckId = 0);
};

}
#endif // PRINTER_H
