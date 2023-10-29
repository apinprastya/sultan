/*
 * purchaseaddpaymentdialog.cpp
 * Copyright 2023, Apin <apin.klas@gmail.com>
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

#include "purchaseaddpaymentdialog.h"
#include "dbutil.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "guiutil.h"
#include "message.h"
#include "preference.h"
#include "ui_purchaseaddpaymentdialog.h"
#include "usersession.h"
#include <QDateTime>

namespace LibGUI {

using namespace LibG;

PurchaseAddPaymentDialog::PurchaseAddPaymentDialog(MessageBus *bus, int purchaseId, float residual,
                                                   const QVariantMap &data, const QVariantMap &oldData, QWidget *parent)
    : QDialog(parent), ui(new Ui::PurchaseAddPaymentDialog), mPurchaseId(purchaseId), mResidual(residual),
      mOldData(oldData) {
    ui->setupUi(this);
    ui->dateEdit->setDate(QDateTime::currentDateTime().date());

    mNumber = data["number"].toString();
    mSupplier = data["suplier"].toString();

    setMessageBus(bus);
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);

    connect(ui->pushCancel, SIGNAL(clicked(bool)), SLOT(cancelClicked()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));

    ui->doubleAmount->setValue(residual);
    ui->labelResidual->setText(tr("Residual: %1").arg(Preference::formatMoney(residual)));

    ui->labelError->hide();
    setWindowTitle(oldData.isEmpty() ? tr("Add new payment") : tr("Update payment"));

    if (!oldData.isEmpty()) {
        ui->lineNumber->setText(oldData["number"].toString());
        ui->dateEdit->setDate(LibDB::DBUtil::sqlDateToDate(oldData["date"].toString()));
        ui->doubleAmount->setValue(-oldData["money_total"].toDouble());
    }
}

PurchaseAddPaymentDialog::~PurchaseAddPaymentDialog()
{
    delete ui;
}

void LibGUI::PurchaseAddPaymentDialog::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
        if (msg->isSuccess()) {
            const QVariantList &l = msg->data("data").toList();
            ui->comboMethod->addItem(tr("Cash"), 0);
            for (int i = 0; i < l.size(); i++) {
                const QVariantMap &data = l[i].toMap();
                int id = data["id"].toInt();
                ui->comboMethod->addItem(data["name"].toString(), id);
            }
            if (!mOldData.isEmpty()) {
                GuiUtil::selectCombo(ui->comboMethod, mOldData["bank_id"].toInt());
            }
        }
    } else if (msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::ADD_PAYMENT)) {
        if (!msg->isSuccess()) {
            const QString &errString = msg->getErrorString();
            if (errString == "payment_too_big") {
                showError(tr("Payment too big"));
                ui->pushSave->setEnabled(true);
                return;
            }
        }
        close();
    } else if (msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::UPDATE_PAYMENT)) {
        if (!msg->isSuccess()) {
            const QString &errString = msg->getErrorString();
            if (errString == "payment_too_big") {
                showError(tr("Payment too big"));
                ui->pushSave->setEnabled(true);
                return;
            }
        }
        close();
    }
}

void PurchaseAddPaymentDialog::cancelClicked() { close(); }

void PurchaseAddPaymentDialog::saveClicked() {
    if (mOldData.isEmpty() && ui->doubleAmount->value() > mResidual) {
        showError(tr("Payment too big"));
        return;
    }
    if (ui->lineNumber->text().isEmpty()) {
        showError(tr("Number can not empty"));
        return;
    }
    ui->labelError->hide();
    if (mOldData.isEmpty()) {
        Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::ADD_PAYMENT);
        QVariantMap data{{"type", TRANSACTION_TYPE::EXPENSE},
                         {"detail", tr("Purchase: %1 (%2)").arg(mNumber).arg(mSupplier)},
                         {"transaction_total", -ui->doubleAmount->value()},
                         {"money_total", -ui->doubleAmount->value()},
                         {"link_type", TRANSACTION_LINK_TYPE::PURCHASE},
                         {"link_id", mPurchaseId},
                         {"number", ui->lineNumber->text()},
                         {"date", ui->dateEdit->dateTime()},
                         {"machine_id", Preference::getInt(SETTING::MACHINE_ID)},
                         {"bank_id", ui->comboMethod->currentData()}};
        msg.setData(data);
        msg.addData("user_id", UserSession::id());
        ui->pushSave->setEnabled(false);
        sendMessage(&msg);
    } else {
        Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::UPDATE_PAYMENT);
        QVariantMap data{{"transaction_total", -ui->doubleAmount->value()},
                         {"money_total", -ui->doubleAmount->value()},
                         {"number", ui->lineNumber->text()},
                         {"date", ui->dateEdit->dateTime()},
                         {"bank_id", ui->comboMethod->currentData()}};
        msg.addData("id", mOldData["id"].toInt());
        msg.addData("purchaseId", mPurchaseId);
        msg.addData("data", data);
        ui->pushSave->setEnabled(false);
        sendMessage(&msg);
    }
}

void PurchaseAddPaymentDialog::showError(const QString &error) {
    ui->labelError->setText(error);
    ui->labelError->show();
}

} // namespace LibGUI
