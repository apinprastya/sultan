/*
 * purchaseadddialog.cpp
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
#include "purchaseadddialog.h"
#include "ui_purchaseadddialog.h"
#include "message.h"
#include "global_constant.h"
#include "guiutil.h"
#include "dbutil.h"
#include "util.h"
#include "flashmessagemanager.h"
#include "preference.h"
#include "global_setting_const.h"
#include "usersession.h"
#include "guiutil.h"
#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchaseAddDialog::PurchaseAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseAddDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->dateCreated->setCalendarPopup(true);
    ui->dateDeadline->setCalendarPopup(true);
    ui->comboPaymentType->addItem(tr("Direct"), PURCHASEPAYMENT::DIRECT);
    ui->comboPaymentType->addItem(tr("With Deadline"), PURCHASEPAYMENT::TEMPO);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->comboPaymentType, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged()));
    connect(ui->lineDiscountFormula, SIGNAL(textChanged(QString)), SLOT(calculateTotal()));
    typeChanged();
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

PurchaseAddDialog::~PurchaseAddDialog()
{
    delete ui;
}

void PurchaseAddDialog::reset()
{
    ui->dateCreated->setDate(QDate::currentDate());
    ui->dateDeadline->setDate(QDate::currentDate().addDays(7));
    ui->lineNumber->clear();
    ui->comboSuplier->setCurrentIndex(0);
    mCurrentSuplier = 0;
    ui->dateCreated->setFocus(Qt::TabFocusReason);
    mId = 0;
    ui->pushSave->setEnabled(true);
    ui->groupBox->hide();
    this->adjustSize();
}

void PurchaseAddDialog::fill(const QVariantMap &data)
{
    ui->dateCreated->setDate(LibDB::DBUtil::sqlDateToDate(data["created_at"].toString()));
    ui->dateDeadline->setDate(LibDB::DBUtil::sqlDateToDate(data["deadline"].toString()));
    ui->lineNumber->setText(data["number"].toString());
    mCurrentSuplier = data["suplier_id"].toInt();
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
    ui->dateCreated->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mTotal = data["total"].toDouble();
    mBankId = data["bank_id"].toInt();
    const QString &discFormula = data["discount_formula"].toString();
    ui->lineDiscountFormula->setText(discFormula);
    if(discFormula.isEmpty()) calculateTotal();
    mId = data["id"].toInt();
    ui->groupBox->show();
    this->adjustSize();
}

void PurchaseAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::SUPLIER)) {
        const QVariantList &list = msg->data("data").toList();
        populateSuplier(list);
    } else if(msg->isType(MSG_TYPE::PURCHASE)) {
        if(msg->isSuccess()) {
            hide();
            if(mId > 0) {
                FlashMessageManager::showMessage(tr("Purchase added successfully"));
                emit successUpdate(mId);
            } else {
                FlashMessageManager::showMessage(tr("Purchase added successfully"));
                emit successAdd();
            }
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            ui->pushSave->setEnabled(true);
        }
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
}

void PurchaseAddDialog::showEvent(QShowEvent *event)
{
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
    QDialog::showEvent(event);
}

void PurchaseAddDialog::populateSuplier(const QVariantList &list)
{
    ui->comboSuplier->clear();
    ui->comboSuplier->addItem(tr("-- Select Suplier --"), 0);
    for(auto &d : list) {
        const QVariantMap &m = d.toMap();
        ui->comboSuplier->addItem(m["name"].toString(), m["id"].toInt());
    }
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
}

void PurchaseAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineNumber << ui->comboSuplier)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill required field"));
        return;
    }
    const QString &discFormula = ui->lineDiscountFormula->text();
    if(discFormula.contains(" ")) {
        QMessageBox::critical(this, tr("Error"), tr("Discount formula can not consist space[s]"));
        return;
    }
    if(!Util::isValidDiscountFormula(discFormula)) {
        QMessageBox::critical(this, tr("Error"), tr("Discount formula not valid"));
        return;
    }
    QVariantMap d;
    d.insert("created_at", ui->dateCreated->date().toString("yyyy-MM-dd"));
    d.insert("number", ui->lineNumber->text());
    d.insert("suplier_id", ui->comboSuplier->currentData());
    d.insert("deadline", ui->dateDeadline->date().toString("yyyy-MM-dd"));
    d.insert("payment_type", ui->comboPaymentType->currentData().toInt());
    d.insert("discount_formula", ui->lineDiscountFormula->text());
    d.insert("user_id", UserSession::id());
    d.insert("machine_id", Preference::getInt(SETTING::MACHINE_ID));
    if(ui->comboPaymentType->currentData().toInt() == PURCHASEPAYMENT::DIRECT)
        d.insert("bank_id", ui->comboBank->currentData());
    Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::INSERT);
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        double discount = Util::calculateDiscount(ui->lineDiscountFormula->text(), mTotal);
        d.insert("discount", discount);
        d.insert("final", mTotal - discount);
        msg.addData("id", mId);
        msg.addData("data", d);
    } else {
        msg.setData(d);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}

void PurchaseAddDialog::typeChanged()
{
    ui->dateDeadline->setEnabled(ui->comboPaymentType->currentData().toInt() != PURCHASEPAYMENT::DIRECT);
    ui->comboBank->setEnabled(ui->comboPaymentType->currentData().toInt() == PURCHASEPAYMENT::DIRECT);
}

void PurchaseAddDialog::calculateTotal()
{
    double disc = Util::calculateDiscount(ui->lineDiscountFormula->text(), mTotal);
    ui->labelTotal->setText(Preference::formatMoney(mTotal));
    ui->labelDiscount->setText(Preference::formatMoney(disc));
    ui->labelFinal->setText(Preference::formatMoney(mTotal - disc));
}
