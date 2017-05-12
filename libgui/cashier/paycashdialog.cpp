/*
 * paycashdialog.cpp
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
#include "paycashdialog.h"
#include "ui_paycashdialog.h"
#include <QLocale>
#include <QMessageBox>

PayCashDialog::PayCashDialog(double total, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PayCashDialog),
    mTotal(total)
{
    ui->setupUi(this);
    ui->labelTotal->setText(QLocale().toString(total));
    ui->doubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->doubleSpinBox->setMinimum(std::numeric_limits<double>::min());
    ui->doubleSpinBox->setValue(total);
    ui->doubleSpinBox->selectAll();
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payClicked()));
}

PayCashDialog::~PayCashDialog()
{
    delete ui;
}

void PayCashDialog::payClicked()
{
    double payment = ui->doubleSpinBox->value();
    if(payment < mTotal) {
        QMessageBox::critical(this, tr("Error Payment"), tr("Payment must bigger or equal to total"));
        return;
    }
    mTotal = payment;
    mIsPayed = true;
    close();
}
