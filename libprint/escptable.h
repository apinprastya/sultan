/*
 * escptable.h
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
#ifndef ESCPTABLE_H
#define ESCPTABLE_H

#include "print_global.h"
#include <QString>
#include <QStringList>

namespace LibPrint {

class PRINTSHARED_EXPORT EscpTable
{
public:
    enum {
        ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
    };
    EscpTable(int width);
    void setColumnWidth(const QList<int> &colWidths);
    inline void setUseBorder(bool border) { mUseBorder = border; }
    void writeHeader(const QStringList &header);
    void writeRow(const QStringList &rowData);
    void setAlign(int column, int alignment);
    void end();
    QString data();

private:
    int mWidth;
    bool mUseBorder;
    QList<int> mColWidth;
    QList<int> mAlign;
    QString mData;
    QStringList mTempData;

    int getAlignment(int col);
    void append(QChar ch, int lenght);
    void write(const QString &val, int column);
    void write(const QString &val, int aligment, int width);
};

}

#endif // ESCPTABLE_H
