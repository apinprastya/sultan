/*
 * addsoldreturndialog.cpp
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
#include "addsoldreturndialog.h"
#include "cashier/transactionlistdialog.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "keyevent.h"
#include "message.h"
#include "preference.h"
#include "ui_addsoldreturndialog.h"
#include "usersession.h"
#include <QDate>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

AddSoldReturnDialog::AddSoldReturnDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddSoldReturnDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->dateResolusion->setDate(QDate::currentDate());
    ui->dateResolusion->setReadOnly(true);
    auto ke = new KeyEvent(this);
    ke->setClickEvent(true);
    ui->lineBarcode->installEventFilter(ke);
    connect(ke, SIGNAL(clicked(QObject *)), SLOT(openTransactionDialog()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculatePrice()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->doubleMoneyReturned, SIGNAL(valueChanged(double)), SLOT(checkBank()));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

AddSoldReturnDialog::~AddSoldReturnDialog() { delete ui; }

void AddSoldReturnDialog::fill(const QVariantMap &data) {
    mId = data["id"].toInt();
    ui->dateResolusion->setReadOnly(false);
    mPrice = data["price"].toDouble();
    ui->doubleCount->setValue(data["count"].toFloat());
    ui->lineBarcode->setEnabled(false);
    mBankSelected = data["bank_id"].toInt();
    ui->groupResolution->setChecked(data["status"].toInt() == PURCHASE_RETURN_STATUS::RETURNED);
    if (data["status"].toInt() == PURCHASE_RETURN_STATUS::RETURNED) {
        ui->dateResolusion->setDate(QDate::fromString(data["return_date"].toString(), "yyyy-MM-dd"));
        ui->doubleItemReturned->setValue(data["item_returned"].toDouble());
        ui->doubleMoneyReturned->setValue(data["money_returned"].toDouble());
    }
    fillField(data);
}

void AddSoldReturnDialog::messageReceived(LibG::Message *msg) {
    if (msg->isType(MSG_TYPE::SOLDRETURN)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(msg->isCommand(MSG_COMMAND::INSERT)
                                                 ? tr("Sold return added successfully")
                                                 : tr("Sold return updated successfully"));
            close();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
    } else if (msg->isType(MSG_TYPE::BANK) && msg->isSuccess()) {
        const QVariantList &l = msg->data("data").toList();
        ui->comboBank->addItem(tr("Cash"), 0);
        for (int i = 0; i < l.size(); i++) {
            const QVariantMap &data = l[i].toMap();
            int id = data["id"].toInt();
            ui->comboBank->addItem(data["name"].toString(), id);
        }
        GuiUtil::selectCombo(ui->comboBank, mBankSelected);
    }
}

void AddSoldReturnDialog::fillField(const QVariantMap &data) {
    mSoldId = data["sold_id"].toInt();
    mSoldItemId = data["sold_item_id"].toInt();
    ui->labelPrice->setText(Preference::formatMoney(mPrice));
    ui->labelSoldNumber->setText(data["sold_number"].toString());
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->labelName->setText(data["name"].toString());
    ui->doubleCount->setMaximum(data["count"].toDouble());
    ui->doubleItemReturned->setMaximum(data["count"].toDouble());
    ui->plainNote->setPlainText(data["note"].toString());
    calculatePrice();
}

void AddSoldReturnDialog::saveClicked() {
    if ((ui->lineBarcode->text().isEmpty() || ui->doubleCount->value() == 0) ||
        (ui->groupResolution->isChecked() && ui->doubleMoneyReturned->value() > 0 &&
         ui->comboBank->currentData().toInt() < 0)) {
        FlashMessageManager::showError(tr("Please fill the field correctly"));
        return;
    }
    Message msg(MSG_TYPE::SOLDRETURN, MSG_COMMAND::INSERT);
    QVariantMap data{{"barcode", ui->lineBarcode->text()},
                     {"name", ui->labelName->text()},
                     {"count", ui->doubleCount->value()},
                     {"price", mPrice},
                     {"sold_item_id", mSoldItemId},
                     {"status", ui->groupResolution->isChecked() ? PURCHASE_RETURN_STATUS::RETURNED
                                                                 : PURCHASE_RETURN_STATUS::UNRETURN},
                     {"sold_id", mSoldId},
                     {"return_date", ui->dateResolusion->date().toString("yyyy-MM-dd")},
                     {"total", (mPrice * ui->doubleCount->value())},
                     {"item_returned", ui->doubleItemReturned->value()},
                     {"money_returned", ui->doubleMoneyReturned->value()},
                     {"bank_id", ui->comboBank->currentData().toInt()},
                     {"note", ui->plainNote->toPlainText()}};
    if (!ui->groupResolution->isChecked())
        data.remove("return_date");
    if (mId < 0) {
        data["user_id"] = UserSession::id();
        msg.setData(data);
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}

void AddSoldReturnDialog::calculatePrice() {
    ui->labelTotal->setText(Preference::formatMoney(mPrice * ui->doubleCount->value()));
}

void AddSoldReturnDialog::openTransactionDialog() {
    if (mId > 0)
        return;
    TransactionListDialog dialog(mMessageBus, this);
    dialog.setType(TransactionListDialog::SoldReturn);
    dialog.exec();
    if (dialog.isOk()) {
        const QVariantMap &data = dialog.getData();
        mSoldItemId = data["id"].toInt();
        mPrice = data["price"].toDouble() - data["discount"].toDouble();
        fillField(data);
        ui->doubleCount->setFocus(Qt::TabFocusReason);
    }
}

void AddSoldReturnDialog::checkBank() { ui->comboBank->setEnabled(ui->doubleMoneyReturned->value() > 0); }
