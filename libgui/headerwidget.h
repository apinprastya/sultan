/*
 * headerwidget.h
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
#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include "gui_global.h"
#include <QWidget>
#include <QVariant>

class QLineEdit;
class QComboBox;
class QDateEdit;

namespace LibGUI {

class GUISHARED_EXPORT HeaderWidget : public QWidget
{
    Q_OBJECT

public:
    enum Type {
        None, LineEdit, Combo, Date, DateStartEnd
    };

    HeaderWidget(int index, int type, const QString &title, QWidget *parent = nullptr);

signals:
    void filterValueChanged(int index, QVariant value);

private:
    int mIndex;
    QLineEdit *mLineEdit = nullptr;
    QComboBox *mComboBox = nullptr;
    QDateEdit *mDateEdit = nullptr;
    QDateEdit *mDateEnd = nullptr;

private slots:
    void lineEditDone();
    void comboChanged();
    void dateChanged();
};

}
#endif // HEADERWIDGET_H
