/*
 * listdialog.cpp
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
#include "listdialog.h"
#include "ui_listdialog.h"
#include <QJsonArray>
#include <QJsonObject>

using namespace LibGUI;

ListDialog::ListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), SLOT(onSelect()));
    connect(ui->pushSelect, SIGNAL(clicked(bool)), SLOT(onSelect()));
}

ListDialog::~ListDialog()
{
    delete ui;
}

void ListDialog::fill(const QJsonArray &arr)
{
    ui->listWidget->clear();
    for(int i = 0; i < arr.size(); i++) {
        const QJsonObject &json = arr[i].toObject();
        auto item = new QListWidgetItem(json.value("name").toString());
        item->setData(Qt::UserRole, json.value("id").toString());
        ui->listWidget->addItem(item);
    }
}

void ListDialog::onSelect()
{
    auto item = ui->listWidget->currentItem();
    if(item != nullptr) {
        mSelectedId = item->data(Qt::UserRole).toString();
    }
    close();
}
