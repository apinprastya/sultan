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
#include "flashmessagemanager.h"
#include "util.h"
#include "preference.h"
#include "item/additemdialog.h"
#include "db_constant.h"
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
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(barcodeDone()));
    connect(ui->lineBarcode, SIGNAL(editingFinished()), SLOT(barcodeDone()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(saveAgainClicked()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculateDiscount()));
    connect(ui->doublePrice, SIGNAL(valueChanged(double)), SLOT(calculateDiscount()));
    connect(ui->lineDiscountFormula, SIGNAL(textChanged(QString)), SLOT(calculateDiscount()));
    connect(ui->pushAddNewItem, SIGNAL(clicked(bool)), SLOT(addNewItemClicked()));
}

PurchaseAddItemDialog::~PurchaseAddItemDialog()
{
    delete ui;
}

void PurchaseAddItemDialog::reset()
{
    GuiUtil::clearAll(QList<QWidget*>() << ui->lineBarcode << ui->labelName <<
                      ui->doubleBuyPrice << ui->doubleCount <<
                      ui->doubleSellPrice << ui->doublePrice);
    GuiUtil::enableWidget(false, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleCount <<
                          ui->doubleSellPrice << ui->doublePrice << ui->pushSave <<
                          ui->pushSaveAgain << ui->lineDiscountFormula);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->lineBarcode->setReadOnly(false);
    ui->lineDiscountFormula->clear();
    mId = 0;
    ui->pushSaveAgain->show();
    ui->pushAddNewItem->hide();
}

void PurchaseAddItemDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    mCurrentStock = data["stock"].toFloat();
    mBuyPrice = data["buy_price"].toDouble();
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineBarcode->setReadOnly(true);
    ui->labelName->setText(data["name"].toString());
    ui->doubleBuyPrice->setValue(data["buy_price"].toDouble());
    ui->doubleCount->setValue(data["count"].toDouble());
    ui->doubleSellPrice->setValue(data["sell_price"].toDouble());
    ui->doublePrice->setValue(data["price"].toDouble());
    ui->lineDiscountFormula->setText(data["discount_formula"].toString());
    ui->doubleCount->setFocus();
    ui->pushSaveAgain->hide();
    ui->pushAddNewItem->hide();
}

void PurchaseAddItemDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            ui->labelName->setText(msg->data("name").toString());
            ui->labelName->setStyleSheet("color: black");
            ui->doubleBuyPrice->setValue(msg->data("buy_price").toDouble());
            ui->doubleSellPrice->setValue(msg->data("sell_price").toDouble());
            GuiUtil::enableWidget(true, QList<QWidget*>() << ui->doubleBuyPrice << ui->doubleCount <<
                                  ui->doubleSellPrice << ui->doublePrice << ui->pushSave <<
                                  ui->pushSaveAgain << ui->lineDiscountFormula);
            ui->doubleCount->setFocus(Qt::TabFocusReason);
            getPurchaseItem(ui->lineBarcode->text());
        } else {
            ui->labelName->setText(tr("Item with barcode not found"));
            ui->labelName->setStyleSheet("color: red");
            ui->pushAddNewItem->show();
            ui->pushAddNewItem->setFocus(Qt::TabFocusReason);
            return;
        }
    } else if(msg->isType(MSG_TYPE::PURCHASE_ITEM)) {
        if(msg->isSuccess()) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Purchase item added successfully"));
                emit addSuccess();
                if(!mIsAgain) hide();
                else {
                    reset();
                }
            } else if(msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Purchase item updated successfully"));
                hide();
                emit updateSuccess(mId);
            } else if(msg->isCommand(MSG_COMMAND::QUERY)) {
                const QVariantList &l = msg->data("data").toList();
                if(!l.isEmpty()) {
                    const QVariantMap &d = l[0].toMap();
                    mId = d["id"].toInt();
                    ui->doubleCount->setValue(d["count"].toFloat());
                    ui->doublePrice->setValue(d["total"].toDouble());
                    ui->lineDiscountFormula->setText(d["discount_formula"].toString());
                    calculateDiscount();
                    ui->doubleCount->selectAll();
                }
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
    if(ui->doubleCount->value() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Count and price can not zero value"));
        return;
    }
    LibG::Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::INSERT);
    QVariantMap data;
    data.insert("barcode", ui->lineBarcode->text());
    data.insert("purchase_id", mPurchaseId);
    data.insert("count", ui->doubleCount->value());
    data.insert("price", ui->doublePrice->value());
    data.insert("total", ui->doublePrice->value() * ui->doubleCount->value());
    data.insert("discount_formula", ui->lineDiscountFormula->text());
    data.insert("discount", mDiscount);
    data.insert("final", mTotal - (ui->doubleCount->value() * mDiscount));
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        msg.setData(data);
    }
    sendMessage(&msg);
}

void PurchaseAddItemDialog::getPurchaseItem(const QString &barcode)
{
    Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::QUERY);
    msg.addFilter("purchase_id", COMPARE::EQUAL, mPurchaseId);
    msg.addFilter("barcode", COMPARE::EQUAL, barcode);
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

void PurchaseAddItemDialog::calculateDiscount()
{
    mDiscount = Util::calculateDiscount(ui->lineDiscountFormula->text(), ui->doublePrice->value());
    mTotal = ui->doublePrice->value() * ui->doubleCount->value();
    ui->labelDiscount->setText(Preference::toString(mDiscount));
    ui->labelPriceDisc->setText(Preference::toString(ui->doublePrice->value() - mDiscount));
    ui->labelTotal->setText(Preference::toString(mTotal - (mDiscount * ui->doubleCount->value())));
    calculateBuyPriceSuggestion();
}

void PurchaseAddItemDialog::calculateBuyPriceSuggestion()
{
    if(mId <= 0) {
        double single = ui->doublePrice->value() - mDiscount;
        if(mCurrentStock == 0) {
            ui->labelBuyPrice->setText(Preference::toString(single));
        } else {
            double val = (single + mBuyPrice) / (mCurrentStock + ui->doubleCount->value());
            ui->labelBuyPrice->setText(Preference::toString(val));
        }
    } else {
        ui->labelBuyPrice->setText("-");
    }
}

void PurchaseAddItemDialog::addNewItemClicked()
{
    AddItemDialog dialog(mMessageBus, this);
    dialog.reset(false);
    dialog.disableAddAgain();
    dialog.setBarcode(ui->lineBarcode->text());
    dialog.exec();
    if(dialog.isSuccess()) {
        barcodeDone();
    }
}
