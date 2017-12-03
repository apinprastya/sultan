/*
 * paymentcashsuccessdialog.cpp
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
#include "paymentcashsuccessdialog.h"
#include "ui_paymentcashsuccessdialog.h"
#include "global_constant.h"
#include "preference.h"

using namespace LibGUI;
using namespace LibG;

PaymentCashSuccessDialog::PaymentCashSuccessDialog(const QVariantMap &data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaymentCashSuccessDialog)
{
    ui->setupUi(this);
    int type = data["payment_type"].toInt();
    setWindowTitle(type == PAYMENT::CASH ? tr("Cash Payment Success") : tr("Cashless Payment Success"));
    double total = data["total"].toDouble();
    double payment = data["payment"].toDouble();
    if(type == PAYMENT::CASH) {
        ui->labelTotal->setText(LibG::Preference::formatMoney(total));
        ui->labelPayment->setText(LibG::Preference::formatMoney(payment));
        ui->labelChange->setText(LibG::Preference::formatMoney(qAbs(total - payment)));
        if(total > payment) {
            ui->labelChange_2->setText(tr("Credit"));
        }
    } else {
        ui->labelTotal_2->setText(tr("Sub Total"));
        ui->labelTotal->setText(Preference::formatMoney(data["subtotal"].toDouble()));
        ui->labelPayment_2->setText(tr("Additional Charge"));
        ui->labelPayment->setText(Preference::formatMoney(data["additional_charge"].toDouble()));
        ui->labelChange_2->setText(tr("Total"));
        ui->labelChange->setText(Preference::formatMoney(total));
    }
}

PaymentCashSuccessDialog::~PaymentCashSuccessDialog()
{
    delete ui;
}
