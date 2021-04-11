/*
 * paycashlessdialog.cpp
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
#include "paycashlessdialog.h"
#include "global_constant.h"
#include "guiutil.h"
#include "message.h"
#include "preference.h"
#include "ui_paycashlessdialog.h"
#include "util.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PayCashlessDialog::PayCashlessDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::PayCashlessDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->comboType->addItem(tr("Debit Card"), CARD_TYPE::DEBIT);
    ui->comboType->addItem(tr("Credit Card"), CARD_TYPE::CREDIT);
    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(calculateTotal()));
    connect(ui->comboBank, SIGNAL(currentIndexChanged(int)), SLOT(calculateTotal()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(payClicked()));
}

PayCashlessDialog::~PayCashlessDialog() { delete ui; }

void PayCashlessDialog::showDialog(const double &total) {
    mTotal = total;
    mAdditionalCharge.clear();
    ui->comboBank->clear();
    ui->labelSubtotal->setText(Preference::formatMoney(total));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
    ui->pushSave->setEnabled(true);
    show();
}

int PayCashlessDialog::getBank() { return ui->comboBank->currentData().toInt(); }

int PayCashlessDialog::getCardType() { return ui->comboType->currentData().toInt(); }

QString PayCashlessDialog::getCardNumber() { return ui->lineCardNumber->text(); }

void PayCashlessDialog::messageReceived(LibG::Message *msg) {
    if (msg->isType(MSG_TYPE::BANK) && msg->isSuccess()) {
        const QVariantList &l = msg->data("data").toList();
        for (int i = 0; i < l.size(); i++) {
            const QVariantMap &data = l[i].toMap();
            int id = data["id"].toInt();
            ui->comboBank->addItem(data["name"].toString(), id);
            mAdditionalCharge.insert(
                id, std::make_tuple(data["debit_charge_formula"].toString(), data["credit_charge_formula"].toString()));
        }
        GuiUtil::selectCombo(ui->comboType, CARD_TYPE::DEBIT);
    }
}

void PayCashlessDialog::calculateTotal() {
    const QString &formula = ui->comboType->currentData().toInt() == CARD_TYPE::DEBIT
                                 ? std::get<0>(mAdditionalCharge[ui->comboBank->currentData().toInt()])
                                 : std::get<1>(mAdditionalCharge[ui->comboBank->currentData().toInt()]);
    mAdditonal = Util::calculateDiscount(formula, mTotal);
    ui->labelAdditional->setText(Preference::formatMoney(mAdditonal));
    ui->labelTotal->setText(Preference::formatMoney(mTotal + mAdditonal));
}

void PayCashlessDialog::payClicked() {
    if (ui->lineCardNumber->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Card number can not empty"));
        return;
    }
    ui->pushSave->setEnabled(false);
    emit requestPay(PAYMENT::NON_CASH, mAdditonal, 0);
}
