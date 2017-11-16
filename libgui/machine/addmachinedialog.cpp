/*
 * addmachinedialog.cpp
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
#include "addmachinedialog.h"
#include "ui_addmachinedialog.h"
#include "flashmessagemanager.h"
#include "message.h"
#include "global_constant.h"
#include "util.h"

using namespace LibGUI;
using namespace LibG;

AddMachineDialog::AddMachineDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMachineDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

AddMachineDialog::~AddMachineDialog()
{
    delete ui;
}

void AddMachineDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->lineCode->setText(data["code"].toString());
    ui->lineName->setText(data["name"].toString());
    setWindowTitle(tr("Update machine"));
}

void AddMachineDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::MACHINE)) {
        if(msg->isSuccess()) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) FlashMessageManager::showMessage(tr("Machine added successfully"));
            else FlashMessageManager::showMessage(tr("Machine updated successfully"));
            close();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
        }
        ui->pushSave->setEnabled(true);
    }
}

void AddMachineDialog::saveClicked()
{
    if(ui->lineCode->text().isEmpty() || ui->lineName->text().isEmpty()) {
        FlashMessageManager::showError(tr("Please fill field correctly"));
        return;
    }
    if(ui->lineCode->text().contains(" ")) {
        FlashMessageManager::showError(tr("Code can not contains space"));
        return;
    }
    QVariantMap d{{"code", Util::capitalize(ui->lineCode->text())}, {"name", Util::capitalize(ui->lineName->text())}};
    Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::INSERT);
    if(mId <= 0) {
        msg.setData(d);
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", d);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
