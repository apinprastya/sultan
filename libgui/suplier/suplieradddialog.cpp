/*
 * suplieradddialog.cpp
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
#include "suplieradddialog.h"
#include "guiutil.h"
#include "ui_suplieradddialog.h"
#include "util.h"
#include <QMessageBox>

using namespace LibGUI;

SuplierAddDialog::SuplierAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SuplierAddDialog) {
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

SuplierAddDialog::~SuplierAddDialog() { delete ui; }

void SuplierAddDialog::reset() {
    ui->lineName->clear();
    ui->lineCode->clear();
    ui->linePhone->clear();
    ui->textAddress->clear();
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = -1;
    setWindowTitle(tr("Add new suplier"));
}

void SuplierAddDialog::fill(const QVariantMap &data) {
    ui->lineName->setText(data["name"].toString());
    ui->lineCode->setText(data["code"].toString());
    ui->linePhone->setText(data["phone"].toString());
    ui->textAddress->setPlainText(data["address"].toString());
    ui->lineEmail->setText(data["email"].toString());
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = data["id"].toInt();
    setWindowTitle(tr("Edit suplier"));
}

void SuplierAddDialog::enableSave() { ui->pushSave->setEnabled(true); }

void SuplierAddDialog::saveClicked() {
    if (GuiUtil::anyEmpty(QList<QWidget *>() << ui->lineName << ui->lineCode)) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill all form"));
        return;
    }
    QVariantMap data;
    data["name"] = LibG::Util::capitalize(ui->lineName->text());
    data["code"] = LibG::Util::capitalize(ui->lineCode->text());
    data["phone"] = ui->linePhone->text();
    data["address"] = ui->textAddress->toPlainText();
    data["email"] = ui->lineEmail->text();
    emit saveData(data, mId);
    ui->pushSave->setDisabled(true);
}
