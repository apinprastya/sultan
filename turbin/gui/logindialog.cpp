/*
 * logindialog.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "logindialog.h"
#include "ui_logindialog.h"
#include "global_constant.h"
#include "message.h"
#include <QStringBuilder>
#include <QDebug>

using namespace LibG;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle(CONSTANT::WINDOW_TITLE.arg(tr("Login")));
    connect(ui->pushLogin, SIGNAL(clicked(bool)), SLOT(loginClicked()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::reset()
{
    ui->lineUsername->clear();
    ui->linePassword->clear();
    ui->lineUsername->setFocus(Qt::TabFocusReason);
    ui->labelError->hide();
}

void LoginDialog::showDialog()
{
    reset();
    show();
}

void LoginDialog::messageReceived(Message *msg)
{
    qDebug() << "ITS COMMING";
}

void LoginDialog::loginClicked()
{
    const QString &username = ui->lineUsername->text();
    const QString &password = ui->linePassword->text();
    if(username.isEmpty() || password.isEmpty()) {
        ui->labelError->setText(tr("fill all field"));
        return;
    }
    LibG::Message msg(MSG_TYPE::USER, MSG_COMMAND::LOGIN);
    msg.addData("username", username);
    msg.addData("password", password);
    sendMessage(&msg);
}
