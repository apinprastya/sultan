/*
 * paycashdialog.h
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#ifndef PAYCASHDIALOG_H
#define PAYCASHDIALOG_H

#include <QDialog>

namespace Ui {
class PayCashDialog;
}

class PayCashDialog : public QDialog
{
    Q_OBJECT

public:
    PayCashDialog(double total, QWidget *parent = 0);
    ~PayCashDialog();
    inline bool isPayed() { return mIsPayed; }
    inline double getPayment() { return mTotal; }

private:
    Ui::PayCashDialog *ui;
    double mTotal;
    bool mIsPayed = false;

private slots:
    void payClicked();
};

#endif // PAYCASHDIALOG_H
