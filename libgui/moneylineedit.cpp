/*
 * moneylineedit.cpp
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
#include "moneylineedit.h"
#include "preference.h"
#include <QDoubleValidator>
#include <QLocale>
#include <QDebug>

using namespace LibGUI;

MoneyLineEdit::MoneyLineEdit(QWidget *parent):
    QLineEdit(parent)
{
    setValidator(new QDoubleValidator(this));
    setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    connect(this, SIGNAL(textChanged(QString)), SLOT(textHasChanged(QString)));
}

MoneyLineEdit::~MoneyLineEdit()
{

}

double MoneyLineEdit::value()
{
    QString val = text();
    QLocale locale;
    if(val.endsWith(locale.decimalPoint())) val = val.replace(locale.decimalPoint(), "");
    val = val.replace(locale.groupSeparator(), "");
    return locale.toDouble(val);
}

void MoneyLineEdit::setValue(double value)
{
    QLocale locale;
    if(locale.decimalPoint() == QChar(','))
        setText(QString::number(value).replace(".", ","));
    else
        setText(QString::number(value));
}

void MoneyLineEdit::textHasChanged(const QString &value)
{
    blockSignals(true);
    QString str = value;
    QLocale locale;
    if(str.endsWith(".") && locale.decimalPoint() == QChar(',')) {
        str = str.left(str.length() - 1).append(locale.decimalPoint());
        setText(str);
    }
    if(!str.endsWith(",") && !mIsNative) {
        str = str.replace(locale.groupSeparator(), "");
        if(str.isEmpty()) setText(str);
        else setText(LibG::Preference::formatMoney(locale.toDouble(str)));
    }
    blockSignals(false);
}
