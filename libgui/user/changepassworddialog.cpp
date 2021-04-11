/*
 * changepassworddialog.cpp
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
#include "changepassworddialog.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "message.h"
#include "messagebus.h"
#include "ui_changepassworddialog.h"
#include "usersession.h"
#include <QCryptographicHash>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

ChangePasswordDialog::ChangePasswordDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::ChangePasswordDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

ChangePasswordDialog::~ChangePasswordDialog() { delete ui; }

void ChangePasswordDialog::messageReceived(LibG::Message *msg) {
    if (msg->isSuccess()) {
        FlashMessageManager::showMessage("Password changed successfully");
        close();
    } else {
        QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        ui->pushSave->setEnabled(true);
    }
}

void ChangePasswordDialog::saveClicked() {
    if (GuiUtil::anyEmpty(QList<QWidget *>() << ui->linePasswordOld << ui->linePasswordNew << ui->linePasswordNewRe)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill all field"));
        return;
    }
    if (ui->linePasswordNew->text().compare(ui->linePasswordNewRe->text())) {
        QMessageBox::critical(this, tr("Error"), tr("New password not match"));
        return;
    }
    LibG::Message msg(MSG_TYPE::USER, MSG_COMMAND::CHANGE_MY_PASSWORD);
    msg.addData(
        "current",
        QString(QCryptographicHash::hash(ui->linePasswordOld->text().toUtf8(), QCryptographicHash::Md5).toHex()));
    msg.addData(
        "new",
        QString(QCryptographicHash::hash(ui->linePasswordNew->text().toUtf8(), QCryptographicHash::Md5).toHex()));
    msg.addData("id", UserSession::id());
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
