/*
 * addingridientdialog.cpp
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
#include "addingridientdialog.h"
#include "ui_addingridientdialog.h"
#include "keyevent.h"
#include "cashier/searchitemdialog.h"
#include "preference.h"
#include "global_constant.h"
#include "message.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

AddIngridientDialog::AddIngridientDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddIngridientDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto e = new KeyEvent(this);
    e->setClickEvent(true);
    ui->lineBarcode->installEventFilter(e);
    connect(e, SIGNAL(clicked(QObject*)), SLOT(barcodeClicked()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculatePrice()));
}

AddIngridientDialog::~AddIngridientDialog()
{
    delete ui;
}

void AddIngridientDialog::setData(const QVariantMap &data)
{
    setWindowTitle(tr("Update ingrigient item"));
    ui->doubleCount->setValue(data["count_link"].toFloat());
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", data["barcode_link"]);
    sendMessage(&msg);
    ui->lineBarcode->setEnabled(false);
    ui->pushSave->setEnabled(false);
}

void AddIngridientDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        mData = msg->data();
        fill();
        ui->pushSave->setEnabled(true);
    }
}

void AddIngridientDialog::fill()
{
    ui->lineBarcode->setText(mData["barcode"].toString());
    ui->labelName->setText(mData["name"].toString());
    ui->doubleCount->setFocus(Qt::TabFocusReason);
    ui->doubleCount->selectAll();
    calculatePrice();
}

void AddIngridientDialog::barcodeClicked()
{
    if(!ui->lineBarcode->isEnabled()) return;
    SearchItemDialog dialog(mMessageBus, true, this);
    dialog.exec();
    if(dialog.isOk()) {
        mData = dialog.getSelectedData();
        if(ui->lineBarcode->text().compare(mData["barcode"].toString()) == 0) {
            QMessageBox::critical(this, tr("Error"), tr("Can not consist its own"));
            return;
        }
        fill();
    }
}

void AddIngridientDialog::saveClicked()
{
    if(ui->lineBarcode->text().isEmpty() || ui->doubleCount->value() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill correctly"));
        return;
    }
    mData["count_link"] = ui->doubleCount->value();
    mData["barcode_link"] = mData["barcode"];
    mData["name_link"] = mData["name"];
    mIsOk = true;
    close();
}

void AddIngridientDialog::calculatePrice()
{
    ui->labelBuyPrice->setText(Preference::formatMoney(ui->doubleCount->value() * mData["buy_price"].toDouble()));
    ui->labelSellPrice->setText(Preference::formatMoney(ui->doubleCount->value() * mData["sell_final"].toDouble()));
}
