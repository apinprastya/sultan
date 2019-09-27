/*
 * barcode.cpp
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
#include "barcode.h"
#include "printer.h"
#include "preference.h"
#include "global_setting_const.h"
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QLocale>
#include <QDebug>

using namespace LibPrint;
using namespace LibG;

Barcode::Barcode()
{
}

bool Barcode::print(const QString &/*name*/, const QString &/*barcode*/, int /*price*/, int /*copies*/)
{
    auto pluginsDir = QDir(qApp->applicationDirPath());
    QFile file(pluginsDir.absoluteFilePath(QLatin1String("barcode.zpl")));
    if(!file.exists()) {
        qDebug() << "file not exist";
        return false;
    }
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "file zpl can not openned";
        return false;
    }
    /*int type = Preference::getInt(SETTING::PRINT_BARCODE_TYPE);
    const QString &printname = Preference::getString(SETTING::PRINT_BARCODE);
    QString zpl = QLatin1String(file.readAll());
    zpl.replace(QLatin1String("{copies}"), QString::number(copies));
    zpl.replace(QLatin1String("{name}"), name);
    zpl.replace(QLatin1String("{barcode}"), barcode);
    zpl.replace(QLatin1String("{price}"), QLocale().toString(price));
    Printer::instance()->print(printname, zpl, type);*/
    return true;
}

bool Barcode::print(const QList<BarcodePrintData> &/*data*/)
{
    auto pluginsDir = QDir(qApp->applicationDirPath());
    QFile file(pluginsDir.absoluteFilePath(QLatin1String("barcode.zpl")));
    if(!file.exists()) {
        qDebug() << "file not exist";
        return false;
    }
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "file zpl can not openned";
        return false;
    }
    /*int type = Preference::getInt(SETTING::PRINT_BARCODE_TYPE);
    const QString &printname = Preference::getString(SETTING::PRINT_BARCODE);
    QString allZpl;
    const QString &zpl = QLatin1String(file.readAll());
    for(int i = 0; i < data.size(); i++) {
        const BarcodePrintData &d = data[i];
        QString temp = zpl;
        temp.replace(QLatin1String("{copies}"), QString::number(d.copies));
        temp.replace(QLatin1String("{name}"), d.name);
        temp.replace(QLatin1String("{barcode}"), d.barcode);
        temp.replace(QLatin1String("{price}"), QLocale().toString(d.price));
        allZpl.append(temp);
    }
    Printer::instance()->print(printname, allZpl, type);*/
    return true;
}
