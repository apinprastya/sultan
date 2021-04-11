/*
 * useraddialog.cpp
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
#include "useradddialog.h"
#include "guiutil.h"
#include "ui_useradddialog.h"
#include <QCryptographicHash>
#include <QMessageBox>

using namespace LibGUI;

UserAddDialog::UserAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::UserAddDialog) {
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

UserAddDialog::~UserAddDialog() { delete ui; }

void UserAddDialog::reset() {
    ui->lineName->clear();
    ui->linePassword->clear();
    ui->linePhone->clear();
    ui->lineRepassword->clear();
    ui->lineUsername->clear();
    ui->textAddress->clear();
    ui->lineUsername->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    ui->linePassword->setEnabled(true);
    ui->lineRepassword->setEnabled(true);
    setWindowTitle(tr("Add new user"));
    mId = -1;
}

void UserAddDialog::fill(const QVariantMap &data) {
    ui->lineUsername->setText(data["username"].toString());
    ui->lineName->setText(data["name"].toString());
    ui->linePhone->setText(data["phone"].toString());
    ui->textAddress->setPlainText(data["address"].toString());
    ui->linePassword->clear();
    ui->linePassword->setDisabled(true);
    ui->lineRepassword->clear();
    ui->lineRepassword->setDisabled(true);
    ui->pushSave->setEnabled(true);
    mId = data["id"].toInt();
    setWindowTitle(tr("Edit user"));
}

void UserAddDialog::enableSave() { ui->pushSave->setEnabled(true); }

void UserAddDialog::saveClicked() {
    if ((mId <= 0 && GuiUtil::anyEmpty(QList<QWidget *>() << ui->lineUsername << ui->linePassword << ui->lineRepassword
                                                          << ui->lineName)) ||
        (mId > 0 && GuiUtil::anyEmpty(QList<QWidget *>() << ui->lineUsername << ui->lineName))) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill all form"));
        return;
    }
    if (ui->linePassword->text().compare(ui->lineRepassword->text())) {
        QMessageBox::warning(this, tr("Error"), tr("Password not match"));
        return;
    }
    QVariantMap data;
    data["username"] = ui->lineUsername->text();
    if (mId <= 0)
        data["password"] =
            QString(QCryptographicHash::hash(ui->linePassword->text().toUtf8(), QCryptographicHash::Md5).toHex());
    data["name"] = ui->lineName->text();
    data["phone"] = ui->linePhone->text();
    data["address"] = ui->textAddress->toPlainText();
    emit saveData(data, mId);
    ui->pushSave->setDisabled(true);
}
