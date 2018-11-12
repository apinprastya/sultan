/*
 * addtransactiondialog.cpp
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
#include "addtransactiondialog.h"
#include "ui_addtransactiondialog.h"
#include "flashmessagemanager.h"
#include "message.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "preference.h"
#include "guiutil.h"
#include "usersession.h"
#include "dbutil.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddTransactionDialog::AddTransactionDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTransactionDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->comboType->addItem("-- Select Type --", -1);
    ui->comboType->addItem(tr("Income"), TRANSACTION_TYPE::INCOME);
    ui->comboType->addItem(tr("Expense"), TRANSACTION_TYPE::EXPENSE);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    ui->doubleTotal->setDecimals(Preference::getInt(SETTING::LOCALE_DECIMAL));
    ui->dateEdit->setDateTime(QDateTime::currentDateTime());
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

AddTransactionDialog::~AddTransactionDialog()
{
    delete ui;
}

void AddTransactionDialog::reset()
{
    ui->comboType->setFocus(Qt::TabFocusReason);
    mId = 0;
}

void AddTransactionDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    const int &type = data["type"].toInt();
    const double &total = data["transaction_total"].toDouble();
    GuiUtil::selectCombo(ui->comboType, type);
    ui->lineNumber->setText(data["number"].toString());
    ui->doubleTotal->setValue(type == TRANSACTION_TYPE::EXPENSE ? -total : total);
    ui->plainDetail->setPlainText(data["detail"].toString());
    ui->dateEdit->setDateTime(LibDB::DBUtil::sqlDateToDateTime(data["date"].toString()));
    mBankId = data["bank_id"].toInt();
}

void AddTransactionDialog::messageReceived(Message *msg)
{
    if(msg->isSuccess()) {
        if(msg->isType(MSG_TYPE::TRANSACTION)) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Transaction added successfully"));
            } else {
                FlashMessageManager::showMessage(tr("Transaction updated successfully"));
            }
            close();
        } else if(msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
            if(msg->isSuccess()) {
                const QVariantList &l = msg->data("data").toList();
                ui->comboBank->addItem(tr("Cash"), 0);
                for(int i = 0; i < l.size(); i++) {
                    const QVariantMap &data = l[i].toMap();
                    int id = data["id"].toInt();
                    ui->comboBank->addItem(data["name"].toString(), id);
                }
                GuiUtil::selectCombo(ui->comboBank, mBankId);
            }
        }
    } else {
        QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        ui->pushSave->setEnabled(true);
    }
}

void AddTransactionDialog::saveClicked()
{
    if(ui->comboType->currentData().toInt() < 0 || ui->doubleTotal->value() == 0 ||
            ui->plainDetail->toPlainText().isEmpty() || ui->lineNumber->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please make sure all field filled"));
        return;
    }
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::INSERT);
    const double &total = ui->comboType->currentData().toInt() == TRANSACTION_TYPE::EXPENSE ? -ui->doubleTotal->value() : ui->doubleTotal->value();
    QVariantMap data{{"type", ui->comboType->currentData()},
                     {"detail", ui->plainDetail->toPlainText()},
                     {"transaction_total", total}, {"money_total", total},
                     {"link_type", TRANSACTION_LINK_TYPE::TRANSACTION},
                     {"number", ui->lineNumber->text()},
                     {"date", ui->dateEdit->dateTime()},
                     {"machine_id", Preference::getInt(SETTING::MACHINE_ID)},
                     {"user_id", UserSession::id()},
                     {"bank_id", ui->comboBank->currentData()}};
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        msg.setData(data);
    }
    ui->pushSave->setEnabled(false);
    sendMessage(&msg);
}
