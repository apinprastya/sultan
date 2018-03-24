/*
 * customercreditpaymentdialog.cpp
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
#include "customercreditpaymentdialog.h"
#include "ui_customercreditpaymentdialog.h"
#include "searchcustomerdialog.h"
#include "keyevent.h"
#include "message.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "db_constant.h"
#include "preference.h"
#include "flashmessagemanager.h"
#include "usersession.h"
#include "escp.h"
#include "guiutil.h"
#include "dbutil.h"
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

CustomerCreditPaymentDialog::CustomerCreditPaymentDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomerCreditPaymentDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto e = new KeyEvent(this);
    e->addConsumeKey(Qt::Key_Tab);
    ui->lineID->installEventFilter(e);
    connect(ui->lineID, SIGNAL(returnPressed()), SLOT(numberDone()));
    connect(e, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(numberDone()));
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payClicked()));
    connect(ui->pushSearch, SIGNAL(clicked(bool)), SLOT(openSearchCustomer()));
    connect(ui->doublePayment, SIGNAL(valueChanged(double)), SLOT(calculateRest()));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

CustomerCreditPaymentDialog::~CustomerCreditPaymentDialog()
{
    delete ui;
}

void CustomerCreditPaymentDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY)) {
        mNumber = ui->lineID->text();
        const QVariantList &l = msg->data("data").toList();
        if(l.isEmpty()) {
            FlashMessageManager::showError(tr("Customer not found"));
            return;
        }
        const QVariantMap d = l.first().toMap();
        mId = d["id"].toInt();
        mCredit = d["credit"].toDouble();
        ui->labelName->setText(d["name"].toString());
        ui->labelPhone->setText(d["phone"].toString());
        ui->labelReward->setText(QString::number(d["reward"].toInt()));
        ui->labelCredit->setText(Preference::formatMoney(d["credit"].toDouble()));
        calculateRest();
        ui->doublePayment->setFocus(Qt::TabFocusReason);
    } else if(msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
        if(msg->isSuccess()) {
            const QVariantList &l = msg->data("data").toList();
            ui->comboBank->addItem(tr("Cash"), 0);
            for(int i = 0; i < l.size(); i++) {
                const QVariantMap &data = l[i].toMap();
                int id = data["id"].toInt();
                ui->comboBank->addItem(data["name"].toString(), id);
            }
        }
    } else if(msg->isTypeCommand(MSG_TYPE::CUSTOMER_CREDIT, MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Customer credit payment successfully added"));
            printData(msg->data());
            close();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
        }
    }
}

void CustomerCreditPaymentDialog::printData(const QVariantMap &d)
{
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    if(type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please setting printer first"));
        return;
    }
    const QString &title = Preference::getString(SETTING::PRINTER_CASHIER_TITLE, "Sultan Minimarket");
    int cpi10 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32);
    int cpi12 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40);

    auto escp = new LibPrint::Escp(LibPrint::Escp::SIMPLE, cpi10, cpi12);
    escp->setCpi10Only(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
    escp->cpi10()->doubleHeight(true)->centerText(title)->newLine()->
            centerText(tr("Debt Payment"))->
            doubleHeight(false)->cpi12()->newLine(2);
    escp->column(QList<int>())->line(QChar('='));
    escp->column(QList<int>{50, 50})->leftText(tr("Cust-ID"))->rightText(mNumber)->newLine();
    escp->column(QList<int>{50, 50})->leftText(tr("Name"))->rightText(ui->labelName->text())->newLine();
    escp->column(QList<int>{50, 50})->leftText(tr("Rest Credit"))->rightText(Preference::formatMoney(mCredit - ui->doublePayment->value()))->newLine();
    escp->column(QList<int>())->line(QChar('-'));
    escp->column(QList<int>{50, 50})->leftText(tr("Date"))->rightText(LibDB::DBUtil::sqlDateToDateTime(d["created_at"].toString()).toString("dd-MM-yy hh:mm"))->newLine();
    escp->leftText(tr("Number"))->rightText(d["number"].toString())->newLine();
    escp->leftText(tr("Payment"))->rightText(Preference::formatMoney(-d["credit"].toDouble()))->newLine();
    escp->column(QList<int>())->leftText(tr("Detail :"))->newLine()->leftText(d["detail"].toString())->newLine();
    escp->line(QChar('-'))->newLine(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    GuiUtil::print(escp->data());
    delete escp;
    if(Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT)) {
        const QString &command = LibPrint::Escp::cutPaperCommand();
        GuiUtil::print(command);
    }
}

void CustomerCreditPaymentDialog::numberDone()
{
    if(ui->lineID->text().isEmpty()) return;
    Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
    msg.addFilter("number", COMPARE::EQUAL, ui->lineID->text());
    sendMessage(&msg);
}

void CustomerCreditPaymentDialog::openSearchCustomer()
{
    SearchCustomerDialog dialog(mMessageBus, this);
    dialog.exec();
    if(dialog.isOk()) {
        const QVariantMap &d = dialog.getSelectedData();
        ui->lineID->setText(d["number"].toString());
        numberDone();
    }
}

void CustomerCreditPaymentDialog::payClicked()
{
    if(ui->lineID->text().compare(mNumber)) {
        FlashMessageManager::showError(tr("Customer number is not correct, please check again"));
        return;
    }
    if(ui->doublePayment->value() == 0) {
        FlashMessageManager::showError(tr("Payment must > 0"));
        return;
    }
    auto now = QDateTime::currentMSecsSinceEpoch() / 10000;
    QVariantMap d;
    d.insert("number", QString::number(now));
    d.insert("customer_id", mId);
    d.insert("user_id", UserSession::id());
    d.insert("detail", tr("Credit Payment"));
    d.insert("credit", -ui->doublePayment->value());
    d.insert("machine_id", Preference::getInt(SETTING::MACHINE_ID));
    d.insert("bank_id", ui->comboBank->currentData().toInt());
    Message msg(MSG_TYPE::CUSTOMER_CREDIT, MSG_COMMAND::INSERT);
    msg.setData(d);
    sendMessage(&msg);
    ui->pushPay->setEnabled(false);
}

void CustomerCreditPaymentDialog::calculateRest()
{
    double rest = mCredit - ui->doublePayment->value();
    ui->labelRest->setText(Preference::formatMoney(rest));
}
