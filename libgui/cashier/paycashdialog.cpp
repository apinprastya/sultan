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
#include "preference.h"
#include <QLocale>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PayCashDialog::PayCashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PayCashDialog)
{
    ui->setupUi(this);
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payClicked()));
}

PayCashDialog::~PayCashDialog()
{
    delete ui;
}

void PayCashDialog::fill(double total)
{
    mTotal = total;
    ui->labelTotal->setText(Preference::toString(total));
    ui->lineEdit->setText(QString::number(total));
    ui->lineEdit->selectAll();
    ui->pushPay->setEnabled(true);
}

void PayCashDialog::payClicked()
{
    double payment = ui->lineEdit->value();
    if(payment < mTotal) {
        QMessageBox::critical(this, tr("Error Payment"), tr("Payment must bigger or equal to total"));
        return;
    }
    ui->pushPay->setEnabled(false);
    emit requestPay(payment);
}
