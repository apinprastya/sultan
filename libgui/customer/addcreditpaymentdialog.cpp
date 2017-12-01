/*
 * addcreditpaymentdialog.cpp
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
#include "addcreditpaymentdialog.h"
#include "ui_addcreditpaymentdialog.h"
#include "preference.h"
#include "global_constant.h"
#include "guiutil.h"
#include "flashmessagemanager.h"
#include "usersession.h"
#include "message.h"
#include "preference.h"
#include "global_setting_const.h"
#include "message.h"
#include <QMessageBox>
#include <QDateTime>

using namespace LibGUI;
using namespace LibG;

AddCreditPaymentDialog::AddCreditPaymentDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCreditPaymentDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

AddCreditPaymentDialog::~AddCreditPaymentDialog()
{
    delete ui;
}

void AddCreditPaymentDialog::fill(int id, double total)
{
    mId = id;
    ui->labelTotal->setText(Preference::formatMoney(total));
    ui->linePayment->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
}

void AddCreditPaymentDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::CUSTOMER_CREDIT, MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Credit payment added successfully"));
            emit paymentSuccess();
            hide();
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
        }
    }
}

void AddCreditPaymentDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->linePayment << ui->plainDetail)) {
        FlashMessageManager::showMessage(tr("Please fill all field"), FlashMessage::Error);
        return;
    }
    auto now = QDateTime::currentMSecsSinceEpoch() / 10000;
    QVariantMap d;
    d.insert("number", QString::number(now));
    d.insert("customer_id", mId);
    d.insert("user_id", UserSession::id());
    d.insert("detail", ui->plainDetail->toPlainText());
    d.insert("credit", -ui->linePayment->value());
    d.insert("machine_id", Preference::getInt(SETTING::MACHINE_ID));
    d.insert("bank_id", ui->comboBank->currentData().toInt());
    Message msg(MSG_TYPE::CUSTOMER_CREDIT, MSG_COMMAND::INSERT);
    msg.setData(d);
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
