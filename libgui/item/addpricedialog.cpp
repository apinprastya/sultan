/*
 * addpricedialog.cpp
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
#include "addpricedialog.h"
#include "ui_addpricedialog.h"
#include "global_constant.h"
#include "preference.h"
#include "global_setting_const.h"
#include "message.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddPriceDialog::AddPriceDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPriceDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->doublePrice->setDecimals(Preference::getInt(SETTING::LOCALE_DECIMAL));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

AddPriceDialog::~AddPriceDialog()
{
    delete ui;
}

void AddPriceDialog::reset()
{
    ui->doubleCount->setValue(0);
    ui->doublePrice->setValue(0);
    ui->pushSave->setEnabled(true);
    ui->doubleCount->setFocus(Qt::TabFocusReason);
    mId = 0;
}

void AddPriceDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->doubleCount->setValue(data["count"].toDouble());
    ui->doublePrice->setValue(data["price"].toDouble());
    ui->doubleCount->setFocus(Qt::TabFocusReason);
}

void AddPriceDialog::setBarcodeName(const QString &barcode, const QString &name)
{
    ui->labelBarcode->setText(barcode);
    ui->labelName->setText(name);
}

void AddPriceDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::SELLPRICE)) {
        if(msg->isSuccess()) {
            hide();
            emit success();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void AddPriceDialog::saveClicked()
{
    const double &count = ui->doubleCount->value();
    const double &price = ui->doublePrice->value();
    if(count == 0 && price == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Make sure value is greater than 0"));
        return;
    }
    QVariantMap data;
    Message msg(MSG_TYPE::SELLPRICE, MSG_COMMAND::INSERT);
    data.insert("count", count);
    data.insert("price", price);
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        data.insert("barcode", ui->labelBarcode->text());
        msg.setData(data);
    }
    sendMessage(&msg);
}
