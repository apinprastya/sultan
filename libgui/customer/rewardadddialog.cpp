/*
 * rewardadddialog.cpp
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
#include "rewardadddialog.h"
#include "ui_rewardadddialog.h"
#include "message.h"
#include "global_constant.h"
#include "flashmessagemanager.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

RewardAddDialog::RewardAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RewardAddDialog)
{
    setMessageBus(bus);
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

RewardAddDialog::~RewardAddDialog()
{
    delete ui;
}

void RewardAddDialog::reset()
{
    ui->spinCount->setValue(0);
    ui->lineDetail->clear();
    ui->spinCount->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = -1;
}

void RewardAddDialog::fill(const QVariantMap &data)
{
    ui->spinCount->setValue(data["count"].toInt());
    ui->lineDetail->setText(data["detail"].toString());
    mId = data["id"].toInt();
}

void RewardAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isSuccess()) {
        if(mId < 0)
            FlashMessageManager::showMessage(tr("Reward added successfully"));
        else
            FlashMessageManager::showMessage(tr("Reward updated successfully"));
        close();
    } else {
        QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
    }
}

void RewardAddDialog::saveClicked()
{
    int count = ui->spinCount->value();
    const QString &detail = ui->lineDetail->text();
    if(count < 0 || detail.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("count must bigger than 0 and detail can not be empty"));
        return;
    }
    Message msg(MSG_TYPE::REWARD, MSG_COMMAND::INSERT);
    QVariantMap data{{"count", count}, {"detail", detail}};
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
