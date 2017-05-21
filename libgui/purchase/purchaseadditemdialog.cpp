/*
 * purchaseadditemdialog.cpp
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
#include "purchaseadditemdialog.h"
#include "ui_purchaseadditemdialog.h"
#include "guiutil.h"
#include "global_constant.h"
#include "message.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

PurchaseAddItemDialog::PurchaseAddItemDialog(LibG::MessageBus *bus, int purchaseId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseAddItemDialog),
    mPurchaseId(purchaseId)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->doubleTotal->setReadOnly(true);
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(barcodeDone()));
    connect(ui->lineBarcode, SIGNAL(editingFinished()), SLOT(barcodeDone()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(saveAgainClicked()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculateTotal()));
    connect(ui->doublePrice, SIGNAL(valueChanged(double)), SLOT(calculateTotal()));
}

PurchaseAddItemDialog::~PurchaseAddItemDialog()
{
    delete ui;
}

void PurchaseAddItemDialog::reset()
{
    GuiUtil::clearAll(QList<QWidget*>() << ui->lineBarcode << ui->labelName <<
                      ui->doubleBuyPrice << ui->doubleCount << ui->doublePrice <<
                      ui->doubleSellPrice << ui->doubleTotal);
    GuiUtil::enableWidget(false, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleCount <<
                          ui->doublePrice << ui->doubleSellPrice << ui->doubleTotal << ui->pushSave << ui->pushSaveAgain);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->lineBarcode->setReadOnly(false);
    mId = 0;
    ui->pushSaveAgain->show();
}

void PurchaseAddItemDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineBarcode->setReadOnly(true);
    ui->labelName->setText(data["name"].toString());
    ui->doubleBuyPrice->setValue(data["buy_price"].toDouble());
    ui->doubleCount->setValue(data["count"].toDouble());
    ui->doublePrice->setValue(data["price"].toDouble());
    ui->doubleSellPrice->setValue(data["sell_price"].toDouble());
    ui->doubleTotal->setValue(data["total"].toDouble());
    ui->doubleCount->setFocus();
    ui->pushSaveAgain->hide();
}

void PurchaseAddItemDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            ui->labelName->setText(msg->data("name").toString());
            ui->doubleBuyPrice->setValue(msg->data("buy_price").toDouble());
            ui->doubleSellPrice->setValue(msg->data("sell_price").toDouble());
            GuiUtil::enableWidget(true, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleCount <<
                                  ui->doublePrice << ui->doubleSellPrice << ui->doubleTotal << ui->pushSave << ui->pushSaveAgain);
            ui->doubleCount->setFocus(Qt::TabFocusReason);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Item not found on the database. Please add first"));
            ui->lineBarcode->setFocus();
            return;
        }
    } else if(msg->isType(MSG_TYPE::PURCHASE_ITEM)) {
        if(msg->isSuccess()) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) {
                emit addSuccess();
                if(!mIsAgain) hide();
                else {
                    reset();
                }
            } else if(msg->isCommand(MSG_COMMAND::UPDATE)) {
                hide();
                emit updateSuccess(mId);
            }
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void PurchaseAddItemDialog::showEvent(QShowEvent *event)
{
    mIsClosing = false;
    QDialog::showEvent(event);
}

void PurchaseAddItemDialog::closeEvent(QCloseEvent *event)
{
    mIsClosing = true;
    QDialog::closeEvent(event);
}

void PurchaseAddItemDialog::save()
{
    if(ui->doubleCount->value() == 0 || ui->doublePrice->value() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Count and price can not zero value"));
        return;
    }
    LibG::Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::INSERT);
    QVariantMap data;
    data.insert("barcode", ui->lineBarcode->text());
    data.insert("purchase_id", mPurchaseId);
    data.insert("count", ui->doubleCount->value());
    data.insert("price", ui->doublePrice->value());
    data.insert("total", ui->doubleTotal->value());
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        msg.setData(data);
    }
    sendMessage(&msg);
}

void PurchaseAddItemDialog::barcodeDone()
{
    if(mIsClosing) return;
    if(ui->lineBarcode->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Make sure barcode is filled"));
        ui->lineBarcode->setFocus();
        return;
    }
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", ui->lineBarcode->text());
    sendMessage(&msg);
}

void PurchaseAddItemDialog::saveAgainClicked()
{
    mIsAgain = true;
    save();
}

void PurchaseAddItemDialog::saveClicked()
{
    mIsAgain = false;
    save();
}

void PurchaseAddItemDialog::calculateTotal()
{
    ui->doubleTotal->setValue(ui->doubleCount->value() * ui->doublePrice->value());
}
