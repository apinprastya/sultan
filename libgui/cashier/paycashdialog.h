/*
 * paycashdialog.h
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
#ifndef PAYCASHDIALOG_H
#define PAYCASHDIALOG_H

#include <functional>
#include <QDialog>

namespace Ui {
class PayCashDialog;
}

namespace LibGUI {

class PayCashDialog : public QDialog
{
    Q_OBJECT

public:
    PayCashDialog(QWidget *parent = nullptr);
    ~PayCashDialog();
    void fill(double total);

private:
    Ui::PayCashDialog *ui;
    double mTotal = 0;
    std::function<bool(double)> mPayCallback;
    void saveTransaction();

private slots:
    void payClicked();
    void saveClicked();

signals:
    void requestPay(int type, double payment, int flag);
};

}
#endif // PAYCASHDIALOG_H
