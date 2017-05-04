/*
 * settingdialog.cpp
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
#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "global_constant.h"

using namespace LibG;

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    ui->comboType->addItem(tr("Server"), APPLICATION_TYPE::SERVER);
    ui->comboType->addItem(tr("Client"), APPLICATION_TYPE::CLIENT);

    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(checkType()));
    connect(ui->pushCheckMysql, SIGNAL(clicked(bool)), SLOT(checkMysql()));
    connect(ui->pushCheckConnection, SIGNAL(clicked(bool)), SLOT(checkConnection()));
    connect(ui->pushCancel, SIGNAL(clicked(bool)), SLOT(cancel()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(save()));

    checkType();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::checkType()
{
    int type = ui->comboType->currentData().toInt();
    ui->groupClient->setEnabled(type != APPLICATION_TYPE::SERVER);
    ui->groupMysql->setEnabled(type == APPLICATION_TYPE::SERVER);
    ui->groupServer->setEnabled(type == APPLICATION_TYPE::SERVER);
}

void SettingDialog::checkMysql()
{

}

void SettingDialog::checkConnection()
{

}

void SettingDialog::cancel()
{

}

void SettingDialog::save()
{

}
