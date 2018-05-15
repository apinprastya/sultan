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
    Escp *setCpi10Only(bool value);
    Escp *setWidth(int width);
    Escp *line(const QChar &ch = QChar(0x2d));
    Escp *newLine(int line = 1);
    Escp *bold(bool bold = true);
    Escp *doubleHeight(bool value = true);
    Escp *doubleWidth(bool value = true);
    Escp *cpi10();
    Escp *cpi12();
    Escp *cpi15();
    Escp *column(const QList<int> col);
    Escp *fullText(const QStringList &str);
    Escp *leftText(const QString &str, bool overflow = false);
    Escp *centerText(const QString &str, bool overflow = false);
    Escp *rightText(const QString &str, bool overflow = false);
    inline QString data() const { return mData; }
    inline int numLine() { return mNumLine; }
    inline void append(const QString &data) { mData.append(data); }
    Escp *openDrawer();
    static QString openDrawerCommand();
    static QString cutPaperCommand();
    inline int width() { return mWidth; }

private:
    int mType;
    int mMaster;
    int mWidth10;
    int mWidth12;
    int mWidth15;
    int mWidth;
    int mCurCol;
    int mRestWidth;
    int mNumLine;
    bool mCpi10Only = false;
    QList<int> mColumn;
    QString mData;

    void init();
    int getCurrentWidth(int col);
    bool isTrue(int flag);
    void writeText(const QString &str, int width, int alignment);
};

}
#endif // ESCP_H
