/*
 * purchaseadddialog.cpp
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
#include "purchaseadddialog.h"
#include "ui_purchaseadddialog.h"
#include <QDebug>

using namespace LibGUI;

PurchaseAddDialog::PurchaseAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseAddDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->dateCreated->setCalendarPopup(true);
    ui->dateDeadline->setCalendarPopup(true);
}

PurchaseAddDialog::~PurchaseAddDialog()
{
    delete ui;
}

void PurchaseAddDialog::reset()
{
}

void PurchaseAddDialog::fill(const QVariantMap &data)
{
    qDebug() << data;
}

void PurchaseAddDialog::messageReceived(LibG::Message *msg)
{

}
