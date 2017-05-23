/*
 * paymentcashsuccessdialog.cpp
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
#include "paymentcashsuccessdialog.h"
#include "ui_paymentcashsuccessdialog.h"
#include "preference.h"

using namespace LibGUI;

PaymentCashSuccessDialog::PaymentCashSuccessDialog(const double &total, const double &payment, const double &change, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaymentCashSuccessDialog)
{
    ui->setupUi(this);
    ui->labelTotal->setText(LibG::Preference::toString(total));
    ui->labelPayment->setText(LibG::Preference::toString(payment));
    ui->labelChange->setText(LibG::Preference::toString(change));
}

PaymentCashSuccessDialog::~PaymentCashSuccessDialog()
{
    delete ui;
}
