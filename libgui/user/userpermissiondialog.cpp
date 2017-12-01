/*
 * userpermissiondialog.cpp
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
#include "userpermissiondialog.h"
#include "ui_userpermissiondialog.h"
#include <QDebug>

static QMap<int, QString> AVAIL_PERMISSION = {
    {9876, "Administrator"}, /*{1, "User"},*/ {2, "Suplier"},
    {3, "Item"}, {4, "Cashier"}, {5, "Category"}, {6, "Purchase"},
    {7, "Report"}, {8, "Customer"}, {9, "Bank"}, {10, "Money Report"},
    {11, "Report Transaction"}, {12, "Check Stock"}, /*{13, "Initial Stock"},*/
    {14, "Unit"}
};

UserPermissionDialog::UserPermissionDialog(const QVariantMap &data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserPermissionDialog)
{
    ui->setupUi(this);
    mId = data["id"].toInt();
    mPermission.fromString(data["permission"].toString());
    QMapIterator<int, QString> i(AVAIL_PERMISSION);
    while (i.hasNext()) {
        i.next();
        auto item = new QListWidgetItem(i.value());
        item->setData(Qt::UserRole, i.key());
        if(mPermission.has(i.key()))
            ui->listSelected->addItem(item);
        else
            ui->listAvailable->addItem(item);
    }
    ui->labelName->setText(data["name"].toString());
    ui->labelUsername->setText(data["username"].toString());
    connect(ui->listAvailable, SIGNAL(doubleClicked(QModelIndex)), SLOT(listAvailableDoubleClicked(QModelIndex)));
    connect(ui->listSelected, SIGNAL(doubleClicked(QModelIndex)), SLOT(listSelectedDoubleClicked(QModelIndex)));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

UserPermissionDialog::~UserPermissionDialog()
{
    delete ui;
}

void UserPermissionDialog::listAvailableDoubleClicked(const QModelIndex &index)
{
    auto item = ui->listAvailable->takeItem(index.row());
    ui->listSelected->addItem(item);
    int val = item->data(Qt::UserRole).toInt();
    mPermission.add(val);
}

void UserPermissionDialog::listSelectedDoubleClicked(const QModelIndex &index)
{
    auto item = ui->listSelected->takeItem(index.row());
    ui->listAvailable->addItem(item);
    int val = item->data(Qt::UserRole).toInt();
    mPermission.rem(val);
}

void UserPermissionDialog::saveClicked()
{
    QVariantMap d{{"permission", mPermission.toString()}};
    emit saveData(d, mId);
    close();
}
