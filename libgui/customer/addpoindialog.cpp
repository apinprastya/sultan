/*
 * addpoindialog.cpp
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
#include "addpoindialog.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "message.h"
#include "preference.h"
#include "ui_addpoindialog.h"
#include "usersession.h"
#include <QDateTime>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddPoinDialog::AddPoinDialog(LibG::MessageBus *bus, int id, const QString &number, int poin, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddPoinDialog), mId(id), mPoin(poin) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelNumber->setText(number);
    ui->labelReward->setText(Preference::formatMoney(poin));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    ui->comboType->addItem(tr("-- Select type --"), -1);
    ui->comboType->addItem(tr("Exchange Reward"), POIN_TYPE::EXCHANGE);
    ui->comboType->addItem(tr("Manual"), POIN_TYPE::MANUAL);
    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged()));
    Message msg(MSG_TYPE::REWARD, MSG_COMMAND::QUERY);
    msg.setSort("count ASC");
    sendMessage(&msg);
    typeChanged();
}

AddPoinDialog::~AddPoinDialog() { delete ui; }

void AddPoinDialog::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::REWARD, MSG_COMMAND::QUERY)) {
        if (msg->isSuccess()) {
            ui->comboExchange->clear();
            ui->comboExchange->addItem(tr("-- Select exchange --"), -1);
            const QVariantList &l = msg->data("data").toList();
            for (int i = 0; i < l.size(); i++) {
                const QVariantMap &data = l[i].toMap();
                ui->comboExchange->addItem(QString("%1 : %2").arg(data["count"].toInt()).arg(data["detail"].toString()),
                                           data["count"].toInt());
            }
        }
    } else if (msg->isTypeCommand(MSG_TYPE::CUSTOMER_POINT, MSG_COMMAND::INSERT)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Poin exchange added successfully"));
            close();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
    }
}

void AddPoinDialog::typeChanged() {
    int val = ui->comboType->currentData().toInt();
    ui->comboExchange->setEnabled(val == POIN_TYPE::EXCHANGE);
    ui->spinReward->setEnabled(val == POIN_TYPE::MANUAL);
    ui->plainDetail->setEnabled(val == POIN_TYPE::MANUAL);
}

void AddPoinDialog::saveClicked() {
    int type = ui->comboType->currentData().toInt();
    if (type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select the type"));
        return;
    }
    auto now = QDateTime::currentMSecsSinceEpoch() / 10000;
    Message msg(MSG_TYPE::CUSTOMER_POINT, MSG_COMMAND::INSERT);
    msg.addData("number", QString::number(now, 16).toUpper());
    msg.addData("customer_id", mId);
    msg.addData("user_id", UserSession::id());
    if (type == POIN_TYPE::EXCHANGE) {
        int curPoin = ui->comboExchange->currentData().toInt();
        if (curPoin < 0) {
            QMessageBox::critical(this, tr("Error"), tr("Please select the exchange item"));
            return;
        }
        if (curPoin > mPoin) {
            QMessageBox::critical(this, tr("Error"), tr("Customer poin has less poin than the required exchange poin"));
            return;
        }
        msg.addData("reward", -curPoin);
        msg.addData("detail", ui->comboExchange->currentText());
    } else {
        if (GuiUtil::anyEmpty(QList<QWidget *>{ui->spinReward, ui->plainDetail})) {
            QMessageBox::critical(this, tr("Error"), tr("Make sure poin and detail are filled"));
            return;
        }
        msg.addData("reward", ui->spinReward->value());
        msg.addData("detail", ui->plainDetail->toPlainText());
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
