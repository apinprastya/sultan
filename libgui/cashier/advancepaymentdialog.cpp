/*
 * advancepaymentdialog.cpp
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
#include "advancepaymentdialog.h"
#include "customer/customer.h"
#include "global_constant.h"
#include "preference.h"
#include "ui_advancepaymentdialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AdvancePaymentDialog::AdvancePaymentDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::AdvancePaymentDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->comboBox->addItem(tr("Cash Payment"), PAYMENT::CASH);
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payClicked()));
    connect(ui->linePayment, SIGNAL(textChanged(QString)), SLOT(paymentValueChanged(QString)));
}

AdvancePaymentDialog::~AdvancePaymentDialog() { delete ui; }

void AdvancePaymentDialog::setup(double total, Customer *cust) {
    mTotal = total;
    mCustomer = cust;
    ui->labelTotal->setText(Preference::formatMoney(total));
    ui->linePayment->setText("0");
    ui->linePayment->setFocus(Qt::TabFocusReason);
    ui->linePayment->selectAll();
    ui->labelCusNumber->setText(cust->number);
    ui->labelCusName->setText(cust->name);
    ui->labelCusReward->setText(QString::number(cust->reward));
    ui->labelCusCredit->setText(Preference::formatMoney(cust->credit));
    ui->pushPay->setEnabled(true);
}

void AdvancePaymentDialog::messageReceived(LibG::Message * /*msg*/) {}

void AdvancePaymentDialog::paymentValueChanged(const QString & /*value*/) {
    double val = ui->linePayment->value();
    ui->labelCredit->setText(Preference::formatMoney(mTotal - val));
}

void AdvancePaymentDialog::payClicked() {
    double val = ui->linePayment->value();
    if (val > mTotal) {
        QMessageBox::critical(this, tr("Error"), tr("Payment can not bigger than total"));
        return;
    }
    if (val < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Payment must equal or greater than 0"));
        return;
    }
    ui->pushPay->setEnabled(false);
    emit payRequested(PAYMENT::CASH, val, 0);
}
