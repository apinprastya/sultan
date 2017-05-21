/*
 * escp.h
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
#ifndef ESCP_H
#define ESCP_H

#include "print_global.h"
#include <QList>
#include <QString>

namespace LibPrint {

class PRINTSHARED_EXPORT Escp
{
public:
    enum {
        CPI = 1,
        BOLD = 8,
        DOUBLE_HEIGHT = 16,
        DOUBLE_WIDTH = 32
    };
    enum {
        SIMPLE,
        FULL
    };
    enum {
        LEFT, CENTER, RIGHT
    };
    Escp(int type, int width10, int width12, int width15 = 0);
    void setWidth(int width);
    void line(const QChar &ch = QChar(0x2d));
    void newLine(int line = 1);
    void bold(bool bold = true);
    void doubleHeight(bool value = true);
    void doubleWidth(bool value = true);
    void cpi10();
    void cpi12();
    void cpi15();
    void column(const QList<int> col);
    void leftText(const QString &str, bool overflow = false);
    void centerText(const QString &str, bool overflow = false);
    void rightText(const QString &str, bool overflow = false);
    inline QString data() const { return mData; }
    inline int numLine() { return mNumLine; }
    inline void append(const QString &data) { mData.append(data); }
    void openDrawer();
    static QString openDrawerCommand();

private:
    int mType;
    int mMaster;
    int mWidth10;
    int mWidth12;
    int mWidth15;
    int mWidth;
    int mCurCol;
    QList<int> mColumn;
    int mRestWidth;
    QString mData;
    int mNumLine;

    void init();
    int getCurrentWidth(int col);
    bool isTrue(int flag);
    void writeText(const QString &str, int width, int alignment);
};

}
#endif // ESCP_H
