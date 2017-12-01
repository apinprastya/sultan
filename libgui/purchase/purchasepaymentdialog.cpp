/*
 * purchasepaymentdialog.cpp
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
#include "purchasepaymentdialog.h"
#include "ui_purchasepaymentdialog.h"
#include "preference.h"
#include "message.h"
#include "global_constant.h"
#include "flashmessagemanager.h"
#include "guiutil.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchasePaymentDialog::PurchasePaymentDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchasePaymentDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->checkPaid, SIGNAL(toggled(bool)), SLOT(paidChanged()));
    paidChanged();
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

PurchasePaymentDialog::~PurchasePaymentDialog()
{
    delete ui;
}

void PurchasePaymentDialog::fill(const QVariantMap &data)
{
    ui->labelSuplier->setText(data["suplier"].toString());
    ui->labelTotal->setText(Preference::formatMoney(data["final"].toDouble()));
    mId = data["id"].toInt();
    mBankId = data["bank_id"].toInt();
    QDate date = data["payment_date"].toDate();
    ui->linePayment->setText(data["payment_number"].toString());
    ui->checkPaid->setChecked(data["status"].toInt() != PAYMENT_STATUS::UNPAID);
    if(date.isValid())
        ui->datePayment->setDate(data["payment_date"].toDate());
    else
        ui->datePayment->setDate(QDate::currentDate());
}

void PurchasePaymentDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::UPDATE)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Purchase payment edited successfully"));
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
    } else if(msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
        if(msg->isSuccess()) {
            const QVariantList &l = msg->data("data").toList();
            ui->comboBank->addItem(tr("Cash"), 0);
            for(int i = 0; i < l.size(); i++) {
                const QVariantMap &data = l[i].toMap();
                int id = data["id"].toInt();
                ui->comboBank->addItem(data["name"].toString(), id);
            }
            GuiUtil::selectCombo(ui->comboBank, mBankId);
        }
    }
}

void PurchasePaymentDialog::saveClicked()
{
    Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::UPDATE);
    QVariantMap data;
    data.insert("payment_number", ui->linePayment->text());
    data.insert("payment_date", ui->datePayment->date());
    data.insert("status", ui->checkPaid->isChecked() ? PAYMENT_STATUS::PAID : PAYMENT_STATUS::UNPAID);
    data.insert("bank_id", ui->comboBank->currentData());
    msg.addData("id", mId);
    msg.addData("data", data);
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}

void PurchasePaymentDialog::paidChanged()
{
    ui->linePayment->setEnabled(ui->checkPaid->isChecked());
    ui->datePayment->setEnabled(ui->checkPaid->isChecked());
}
