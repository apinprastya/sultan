/*
 * paycashdialog.cpp
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
#include "paycashdialog.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "preference.h"
#include "ui_paycashdialog.h"
#include "util.h"
#include <QDebug>
#include <QLocale>
#include <QMessageBox>
#include <QShortcut>

using namespace LibGUI;
using namespace LibG;

PayCashDialog::PayCashDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PayCashDialog) {
    ui->setupUi(this);
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payClicked()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    new QShortcut(QKeySequence(Qt::Key_F4), this, SLOT(saveClicked()));
}

PayCashDialog::~PayCashDialog() { delete ui; }

void PayCashDialog::fill(double total) {
    mTotal = total;
    ui->labelTotal->setText(Preference::formatMoney(total));
    // ui->lineEdit->setText(QString::number(total, 'f', Preference::getInt(SETTING::LOCALE_DECIMAL)));
    ui->lineEdit->setValue(total);
    ui->pushPay->setEnabled(true);
    ui->pushSave->setEnabled(true);
    ui->lineEdit->selectAll();
}

bool PayCashDialog::saveTransaction() {
    double payment = ui->lineEdit->value();
    if (Util::roundDouble(payment) < Util::roundDouble(mTotal)) {
        QMessageBox::critical(this, tr("Error Payment"), tr("Payment must bigger or equal to total"));
        return false;
    }
    ui->pushPay->setEnabled(false);
    ui->pushSave->setEnabled(false);
    return true;
}

void PayCashDialog::payClicked() {
    if(saveTransaction()) {
        emit requestPay(PAYMENT::CASH, ui->lineEdit->value(), 0);
    }
}

void PayCashDialog::saveClicked() {
    if(saveTransaction()) {
        emit requestPay(PAYMENT::CASH, ui->lineEdit->value(), 1);
    }
}
