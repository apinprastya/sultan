/*
 * returnresolutiondialog.cpp
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
#include "returnresolutiondialog.h"
#include "dbutil.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "message.h"
#include "preference.h"
#include "ui_returnresolutiondialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

ReturnResolutionDialog::ReturnResolutionDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::ReturnResolutionDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

ReturnResolutionDialog::~ReturnResolutionDialog() { delete ui; }

void ReturnResolutionDialog::fill(const QVariantMap &data) {
    mId = data["id"].toInt();
    ui->labelBarcode->setText(data["barcode"].toString());
    ui->labelName->setText(data["name"].toString());
    ui->labelCount->setText(QString::number(data["count"].toFloat()));
    ui->labelPrice->setText(Preference::formatMoney(data["price"].toDouble()));
    ui->labelSuplier->setText(data["suplier"].toString());
    ui->labelTotal->setText(Preference::formatMoney(data["total"].toDouble()));
    ui->doubleItem->setMaximum(data["count"].toFloat());
    ui->doubleItem->setValue(data["item_returned"].toFloat());
    ui->doubleMoney->setValue(data["money_returned"].toDouble());
    ui->doubleMoney->setMaximum(data["total"].toDouble());
    ui->checkReturned->setChecked(data["status"] == PURCHASE_RETURN_STATUS::RETURNED);
    QDate date = LibDB::DBUtil::sqlDateToDate(data["return_date"].toString());
    ui->dateReturned->setDate(date.isValid() ? date : QDate::currentDate());
}

void ReturnResolutionDialog::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::PURCHASE_RETURN, MSG_COMMAND::UPDATE)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Return item updated successfully"));
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
    } else if (msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
        ui->comboBank->addItem("Cash", 0);
        const QVariantList &l = msg->data("data").toList();
        for (auto v : l) {
            const QVariantMap &m = v.toMap();
            ui->comboBank->addItem(m["name"].toString(), m["id"]);
        }
    }
}

void ReturnResolutionDialog::saveClicked() {
    QVariantMap data{{"status", ui->checkReturned->isChecked() ? PURCHASE_RETURN_STATUS::RETURNED
                                                               : PURCHASE_RETURN_STATUS::UNRETURN},
                     {"item_returned", ui->doubleItem->value()},
                     {"money_returned", ui->doubleMoney->value()},
                     {"return_date", ui->dateReturned->date()},
                     {"bank_id", ui->comboBank->currentData()}};
    Message msg(MSG_TYPE::PURCHASE_RETURN, MSG_COMMAND::UPDATE);
    msg.addData("id", mId);
    msg.addData("data", data);
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
