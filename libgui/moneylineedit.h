/*
 * moneylineedit.h
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
#ifndef MONEYLINEEDIT_H
#define MONEYLINEEDIT_H

#include "gui_global.h"
#include <QLineEdit>

namespace LibGUI {

class GUISHARED_EXPORT MoneyLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    MoneyLineEdit(QWidget *parent = nullptr);
    ~MoneyLineEdit();
    double value();
    void setValue(double value);
    inline void setNative(bool value) { mIsNative = value; }

  private:
    bool mIsNative = false;

  private slots:
    void textHasChanged(const QString &value);
};

} // namespace LibGUI
#endif // MONEYLINEEDIT_H
