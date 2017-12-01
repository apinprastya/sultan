/*
 * settingpoinadddialog.cpp
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
#include "settingpoinadddialog.h"
#include "ui_settingpoinadddialog.h"
#include "global_constant.h"
#include "message.h"
#include "flashmessagemanager.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

SettingPoinAddDialog::SettingPoinAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingPoinAddDialog)
{
    setMessageBus(bus);
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

SettingPoinAddDialog::~SettingPoinAddDialog()
{
    delete ui;
}

void SettingPoinAddDialog::reset()
{
    ui->doubleTotal->setValue(0);
    ui->spinPoin->setValue(0);
    ui->doubleTotal->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
}

void SettingPoinAddDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->doubleTotal->setValue(data["total"].toDouble());
    ui->spinPoin->setValue(data["poin"].toInt());
}

void SettingPoinAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isSuccess()) {
        FlashMessageManager::showMessage(tr("Reward poin setting addedd successfully"));
        close();
    } else {
        FlashMessageManager::showError(msg->data("error").toString());
        ui->pushSave->setEnabled(true);
    }
}

void SettingPoinAddDialog::saveClicked()
{
    const double &total = ui->doubleTotal->value();
    const int &poin = ui->spinPoin->value();
    if(total == 0 || poin == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Total and point must greater than 0"));
        return;
    }
    QVariantMap data;
    data.insert("total", total);
    data.insert("poin", poin);
    Message msg(MSG_TYPE::REWARD_POIN, MSG_COMMAND::INSERT);
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        msg.setData(data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
