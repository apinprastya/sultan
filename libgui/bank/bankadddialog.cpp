/*
 * bankadddialog.cpp
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
#include "bankadddialog.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "message.h"
#include "ui_bankadddialog.h"
#include "util.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

BankAddDialog::BankAddDialog(LibG::MessageBus *bus, QWidget *parent) : QDialog(parent), ui(new Ui::BankAddDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

BankAddDialog::~BankAddDialog() { delete ui; }

void BankAddDialog::reset() {
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = 0;
}

void BankAddDialog::fill(const QVariantMap &data) {
    mId = data["id"].toInt();
    ui->lineName->setText(data["name"].toString());
    ui->lineDebit->setText(data["debit_charge_formula"].toString());
    ui->lineCredit->setText(data["credit_charge_formula"].toString());
}

void BankAddDialog::messageReceived(LibG::Message *msg) {
    if (msg->isType(MSG_TYPE::BANK)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Bank added successfully"));
            close();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
    }
}

void BankAddDialog::saveClicked() {
    if (ui->lineName->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Name can not empty"));
        return;
    }
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::INSERT);
    QVariantMap d{{"name", Util::capitalize(ui->lineName->text())},
                  {"debit_charge_formula", ui->lineDebit->text()},
                  {"credit_charge_formula", ui->lineCredit->text()}};
    if (mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", d);
    } else {
        msg.setData(d);
    }
    sendMessage(&msg);
}
