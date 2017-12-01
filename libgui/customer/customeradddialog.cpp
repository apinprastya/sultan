/*
 * customeradddialog.cpp
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
#include "customeradddialog.h"
#include "ui_customeradddialog.h"
#include "guiutil.h"
#include "global_constant.h"
#include "message.h"
#include "util.h"
#include "flashmessagemanager.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

CustomerAddDialog::CustomerAddDialog(MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomerAddDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

CustomerAddDialog::~CustomerAddDialog()
{
    delete ui;
}

void CustomerAddDialog::reset()
{
    GuiUtil::clearAll(QList<QWidget*>() << ui->lineName << ui->lineNumber << ui->linePhone << ui->plainAddress);
    ui->lineNumber->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = -1;
}

void CustomerAddDialog::fill(const QVariantMap &data)
{
    reset();
    mId = data["id"].toInt();
    ui->lineName->setText(data["name"].toString());
    ui->lineNumber->setText(data["number"].toString());
    ui->linePhone->setText(data["phone"].toString());
    ui->plainAddress->setPlainText(data["address"].toString());
    ui->lineEmail->setText(data["email"].toString());
}

void CustomerAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isSuccess()) {
        if(msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::INSERT)) {
            FlashMessageManager::showMessage(tr("Customer added successfully"));
            emit customerAdded();
            hide();
        } else if(msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::UPDATE)) {
            FlashMessageManager::showMessage(tr("Customer updated successfully"));
            emit customerUpdated(mId);
            hide();
        }
    } else {
        QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        ui->pushSave->setEnabled(true);
    }
}

void CustomerAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineNumber << ui->lineName)) {
        QMessageBox::critical(this, tr("Error"), tr("Number and name are required"));
        return;
    }
    Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::INSERT);
    QVariantMap data;
    data.insert("number", Util::capitalize(ui->lineNumber->text()));
    data.insert("name", Util::capitalize(ui->lineName->text()));
    data.insert("phone", ui->linePhone->text());
    data.insert("address", ui->plainAddress->toPlainText());
    data.insert("email", ui->lineEmail->text());
    if(mId < 0) {
        msg.setData(data);
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
