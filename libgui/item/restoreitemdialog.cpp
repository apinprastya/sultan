/*
 * restoreitemdialog.cpp
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
#include "restoreitemdialog.h"
#include "ui_restoreitemdialog.h"

using namespace LibGUI;

RestoreItemDialog::RestoreItemDialog(const QVariantMap &data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestoreItemDialog)
{
    ui->setupUi(this);
    ui->labelBarcode->setText(data["barcode"].toString());
    ui->labelName->setText(data["name"].toString());
    ui->labelUnit->setText(data["unit"].toString());
    ui->labelCategory->setText(data["category"].toString());
    ui->labelSuplier->setText(data["suplier"].toString());
    connect(ui->pushRestore, SIGNAL(clicked(bool)), SLOT(restoreClicked()));
}

RestoreItemDialog::~RestoreItemDialog()
{
    delete ui;
}

void RestoreItemDialog::restoreClicked()
{
    mIsOk = true;
    close();
}
