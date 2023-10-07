/*
 * purchaseadditemdialog.cpp
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
#include "purchaseadditemdialog.h"
#include "cashier/searchitemdialog.h"
#include "db_constant.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "item/additemdialog.h"
#include "keyevent.h"
#include "message.h"
#include "preference.h"
#include "ui_purchaseadditemdialog.h"
#include "util.h"
#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchaseAddItemDialog::PurchaseAddItemDialog(LibG::MessageBus *bus, int purchaseId, QWidget *parent)
    : QDialog(parent), ui(new Ui::PurchaseAddItemDialog), mPurchaseId(purchaseId) {
    ui->setupUi(this);
    setMessageBus(bus);
    auto ke = new KeyEvent(this);
    ke->addConsumeKey(Qt::Key_Return);
    ke->addConsumeKey(Qt::Key_Enter);
    ke->addConsumeKey(Qt::Key_Tab);
    ui->lineBarcode->installEventFilter(ke);
    connect(ke, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(barcodeDone()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(saveAgainClicked()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculateDiscount()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(unitPriceChanged()));
    connect(ui->doublePrice, SIGNAL(valueChanged(double)), SLOT(unitPriceChanged()));
    connect(ui->doubleTotal, SIGNAL(valueChanged(double)), SLOT(totalPriceChanged()));
    connect(ui->lineDiscountFormula, SIGNAL(textChanged(QString)), SLOT(calculateDiscount()));
    connect(ui->pushAddNewItem, SIGNAL(clicked(bool)), SLOT(addNewItemClicked()));
    connect(ui->doubleSellPrice, SIGNAL(valueChanged(double)), SLOT(calculateMargin()));
    connect(ui->doubleBuyPrice, SIGNAL(valueChanged(double)), SLOT(calculateMargin()));
    connect(ui->pushSearch, SIGNAL(clicked(bool)), SLOT(openSearchItem()));
    ui->doubleBuyPrice->setEnabled(false);
}

PurchaseAddItemDialog::~PurchaseAddItemDialog() { delete ui; }

void PurchaseAddItemDialog::reset() {
    GuiUtil::clearAll(QList<QWidget *>() << ui->lineBarcode << ui->labelName << ui->doubleBuyPrice << ui->doubleCount
                                         << ui->doubleSellPrice << ui->doublePrice << ui->doubleTotal);
    GuiUtil::enableWidget(false, QList<QWidget *>() << ui->doubleCount << ui->doublePrice << ui->pushSave
                                                    << ui->pushSaveAgain << ui->lineDiscountFormula << ui->doubleTotal);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->lineBarcode->setReadOnly(false);
    ui->lineDiscountFormula->clear();
    mId = 0;
    ui->pushSaveAgain->show();
    ui->pushAddNewItem->hide();
}

void PurchaseAddItemDialog::fill(const QVariantMap &data) {
    mId = data["id"].toInt();
    // mBuyPrice = data["buy_price"].toDouble();
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineBarcode->setReadOnly(true);
    ui->labelName->setText(data["name"].toString());
    ui->doubleCount->setValue(data["count"].toDouble());
    mOldCount = data["count"].toDouble();
    ui->doublePrice->setValue(data["price"].toDouble());
    ui->lineDiscountFormula->setText(data["discount_formula"].toString());
    ui->doubleCount->setFocus();
    ui->pushSaveAgain->hide();
    ui->pushAddNewItem->hide();
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", ui->lineBarcode->text());
    sendMessage(&msg);
}

void PurchaseAddItemDialog::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if (msg->isSuccess()) {
            int flag = msg->data("flag").toInt();
            if ((flag & ITEM_FLAG::PURCHASE) == 0) {
                QMessageBox::critical(this, tr("Error"), tr("Item is not purchaseable"));
                ui->lineBarcode->selectAll();
                return;
            }
            mLastBarcode = ui->lineBarcode->text();
            mSellPrice = msg->data("sell_price").toDouble();
            mSellDiscountFormula = msg->data("sell_discount_formula").toString();
            mSellDiscount = msg->data("sell_discount").toDouble();
            mSellFinal = msg->data("sell_final").toDouble();
            mSellCount = msg->data("sell_count").toFloat();
            mBuyPrice = msg->data("buy_price").toDouble();
            mCurrentStock = msg->data("stock").toFloat();
            ui->labelName->setText(msg->data("name").toString());
            ui->labelName->setStyleSheet("color: black");
            ui->doubleBuyPrice->setValue(mBuyPrice);
            ui->doubleSellPrice->setValue(mSellPrice);
            GuiUtil::enableWidget(true, QList<QWidget *>()
                                            << ui->doubleCount << ui->doublePrice << ui->pushSave << ui->pushSaveAgain
                                            << ui->lineDiscountFormula << ui->doubleTotal);
            ui->doubleCount->setFocus(Qt::TabFocusReason);
            getPurchaseItem(ui->lineBarcode->text());
        } else {
            ui->labelName->setText(tr("Item with barcode not found"));
            ui->labelName->setStyleSheet("color: red");
            ui->pushAddNewItem->show();
            ui->pushAddNewItem->setFocus(Qt::TabFocusReason);
            return;
        }
    } else if (msg->isType(MSG_TYPE::PURCHASE_ITEM)) {
        if (msg->isSuccess()) {
            if (msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Purchase item added successfully"));
                emit addSuccess();
                if (!mIsAgain)
                    hide();
                else {
                    reset();
                }
            } else if (msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Purchase item updated successfully"));
                hide();
                emit updateSuccess(mId);
            } else if (msg->isCommand(MSG_COMMAND::QUERY)) {
                const QVariantList &l = msg->data("data").toList();
                if (!l.isEmpty()) {
                    const QVariantMap &d = l[0].toMap();
                    mId = d["id"].toInt();
                    ui->doubleCount->setValue(d["count"].toFloat());
                    ui->doublePrice->setValue(d["price"].toDouble());
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

void PurchaseAddItemDialog::showEvent(QShowEvent *event) {
    mIsClosing = false;
    QDialog::showEvent(event);
}

void PurchaseAddItemDialog::closeEvent(QCloseEvent *event) {
    mIsClosing = true;
    QDialog::closeEvent(event);
}

void PurchaseAddItemDialog::save() {
    if (ui->doubleCount->value() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Count and price can not zero value"));
        return;
    }
    if (mLastBarcode != ui->lineBarcode->text()) {
        QMessageBox::critical(this, tr("Error"), tr("Barcode has problem, please redo the input"));
        return;
    }
    LibG::Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::INSERT);
    QVariantMap data;
    data.insert("count", ui->doubleCount->value());
    data.insert("price", ui->doublePrice->value());
    data.insert("total", ui->doublePrice->value() * ui->doubleCount->value());
    data.insert("discount_formula", ui->lineDiscountFormula->text());
    data.insert("discount", mDiscount);
    data.insert("final", mTotal - (ui->doubleCount->value() * mDiscount));
    data.insert("buy_price", ui->doubleBuyPrice->value());
    if (mSellPrice != ui->doubleSellPrice->value()) {
        data.insert("sell_price", ui->doubleSellPrice->value());
        data.insert("sell_count", mSellCount);
        data.insert("sell_discount", mSellDiscount);
        data.insert("sell_final", mSellFinal);
    }
    if (mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    } else {
        data.insert("barcode", ui->lineBarcode->text());
        data.insert("purchase_id", mPurchaseId);
        msg.setData(data);
    }
    sendMessage(&msg);
}

void PurchaseAddItemDialog::getPurchaseItem(const QString &barcode) {
    Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::QUERY);
    msg.addFilter("purchase_id", COMPARE::EQUAL, mPurchaseId);
    msg.addFilter("barcode", COMPARE::EQUAL, barcode);
    sendMessage(&msg);
}

void PurchaseAddItemDialog::barcodeDone() {
    if (mIsClosing)
        return;
    if (ui->lineBarcode->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Make sure barcode is filled"));
        ui->lineBarcode->setFocus();
        return;
    }
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", ui->lineBarcode->text());
    sendMessage(&msg);
}

void PurchaseAddItemDialog::saveAgainClicked() {
    mIsAgain = true;
    save();
}

void PurchaseAddItemDialog::saveClicked() {
    mIsAgain = false;
    save();
}

void PurchaseAddItemDialog::unitPriceChanged() {
    ui->doubleTotal->blockSignals(true);
    ui->doubleTotal->setValue(ui->doubleCount->value() * ui->doublePrice->value());
    ui->doubleTotal->blockSignals(false);
    calculateDiscount();
}

void PurchaseAddItemDialog::totalPriceChanged() {
    ui->doublePrice->blockSignals(true);
    ui->doublePrice->setValue(ui->doubleTotal->value() / ui->doubleCount->value());
    ui->doublePrice->blockSignals(false);
    calculateDiscount();
}

void PurchaseAddItemDialog::calculateDiscount() {
    mDiscount = Util::calculateDiscount(ui->lineDiscountFormula->text(), ui->doublePrice->value());
    mTotal = ui->doublePrice->value() * ui->doubleCount->value();
    ui->labelDiscount->setText(Preference::formatMoney(mDiscount));
    ui->labelPriceDisc->setText(Preference::formatMoney(ui->doublePrice->value() - mDiscount));
    ui->labelTotal->setText(Preference::formatMoney(mTotal - (mDiscount * ui->doubleCount->value())));
    calculateBuyPriceSuggestion();
}

void PurchaseAddItemDialog::calculateBuyPriceSuggestion() {
    double single = ui->doublePrice->value() - mDiscount;
    if (mId <= 0) {
        if (mCurrentStock <= 0) {
            ui->doubleBuyPrice->setValue(single);
        } else {
            double val = ((mCurrentStock * mBuyPrice) + (single * ui->doubleCount->value())) /
                         (mCurrentStock + ui->doubleCount->value());
            ui->doubleBuyPrice->setValue(val);
        }
    } else {
        float diff = ui->doubleCount->value() - mOldCount;
        double val = ((mCurrentStock * mBuyPrice) + (single * diff)) / (mCurrentStock + diff);
        ui->doubleBuyPrice->setValue(val);
    }
}

void PurchaseAddItemDialog::addNewItemClicked() {
    AddItemDialog dialog(mMessageBus, this);
    dialog.reset(false);
    dialog.disableAddAgain();
    dialog.setBarcode(ui->lineBarcode->text());
    dialog.exec();
    if (dialog.isSuccess()) {
        barcodeDone();
    }
}

void PurchaseAddItemDialog::calculateMargin() {
    mSellDiscount = Util::calculateDiscount(mSellDiscountFormula, ui->doubleSellPrice->value());
    mSellFinal = ui->doubleSellPrice->value() - mSellDiscount;
    double diff = mSellFinal - ui->doubleBuyPrice->value();
    double p = 100;
    if (ui->doubleBuyPrice->value() != 0)
        p = diff * 100 / ui->doubleBuyPrice->value();
    ui->labelSellDiscount->setText(Preference::formatMoney(mSellDiscount));
    ui->labelSellFinal->setText(Preference::formatMoney(mSellFinal));
    ui->labelMargin->setText(QString("%1 (%2%)").arg(Preference::formatMoney(diff)).arg(QString::number(p, 'f', 2)));
}

void PurchaseAddItemDialog::openSearchItem() {
    SearchItemDialog dialog(mMessageBus, true, this);
    dialog.exec();
    if (dialog.isOk()) {
        ui->lineBarcode->setText(dialog.getSelectedData()["barcode"].toString());
        barcodeDone();
    }
}
