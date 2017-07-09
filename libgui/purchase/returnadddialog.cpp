/*
 * returnadddialog.cpp
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
#include "returnadddialog.h"
#include "ui_returnadddialog.h"
#include "keyevent.h"
#include "message.h"
#include "global_constant.h"
#include "guiutil.h"
#include "purchaseitemselectiondialog.h"
#include "preference.h"
#include "flashmessagemanager.h"
#include "usersession.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

ReturnAddDialog::ReturnAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReturnAddDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto event = new KeyEvent(this);
    event->setFocusEvent(true);
    ui->linePurchaseItem->installEventFilter(event);
    connect(event, SIGNAL(focused(QObject*)), SLOT(purchaseItemFocused()));
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculateTotal()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(saveClicked()));

    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
}

ReturnAddDialog::~ReturnAddDialog()
{
    delete ui;
}

void ReturnAddDialog::reset()
{
    if(!mIsAddAgain) ui->comboSuplier->setCurrentIndex(0);
    ui->comboSuplier->setFocus(Qt::TabFocusReason);
    ui->doubleCount->setValue(0);
    ui->plainNote->clear();
    ui->pushSave->setEnabled(true);
    ui->pushSaveAgain->setEnabled(true);
    ui->linePurchaseItem->clear();
    ui->labelName->clear();
    ui->labelPrice->clear();
    ui->labelTotal->clear();
    mId = 0;
}

void ReturnAddDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    mItem.id = data["purchase_item_id"].toInt();
    mItem.barcode = data["barcode"].toString();
    mItem.name = data["name"].toString();
    mItem.price = data["price"].toDouble();
    mItem.discount = 0;
    mCurrentSuplier = data["suplier_id"].toInt();
    ui->doubleCount->setValue(data["count"].toFloat());
    ui->plainNote->setPlainText(data["note"].toString());
    ui->linePurchaseItem->setEnabled(false);
    fillField();
}

void ReturnAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::SUPLIER)) {
        const QVariantList &list = msg->data("data").toList();
        populateSuplier(list);
    } else if(msg->isType(MSG_TYPE::PURCHASE_RETURN)) {
        if(msg->isSuccess()) {
            if(msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Return added successfully"));
            } else if(msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Return updated successfully"));
            }
            ui->linePurchaseItem->setEnabled(true);
            if(mIsAddAgain) reset();
            else close();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            ui->pushSave->setEnabled(true);
            ui->pushSaveAgain->setEnabled(true);
        }
    }
}

void ReturnAddDialog::populateSuplier(const QVariantList &list)
{
    ui->comboSuplier->clear();
    ui->comboSuplier->addItem(tr("-- Select Suplier --"), 0);
    for(auto &d : list) {
        const QVariantMap &m = d.toMap();
        ui->comboSuplier->addItem(m["name"].toString(), m["id"].toInt());
    }
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
}

void ReturnAddDialog::fillField()
{
    ui->linePurchaseItem->setText(mItem.barcode);
    ui->labelName->setText(mItem.name);
    ui->labelPrice->setText(Preference::toString(mItem.price - mItem.discount));
    ui->doubleCount->setFocus(Qt::TabFocusReason);
    ui->doubleCount->selectAll();
    calculateTotal();
}

void ReturnAddDialog::purchaseItemFocused()
{
    int suplier = ui->comboSuplier->currentData().toInt();
    if(suplier <= 0) return;
    PurchaseItemSelectionDialog dialog(mMessageBus, suplier, &mItem, this);
    dialog.exec();
    if(mItem.barcode.isEmpty()) {
        close();
    } else {
        fillField();
    }
}

void ReturnAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->linePurchaseItem << ui->doubleCount)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill correctly"));
        return;
    }
    auto button = static_cast<QPushButton*>(QObject::sender());
    mIsAddAgain = button == ui->pushSaveAgain;
    Message msg(MSG_TYPE::PURCHASE_RETURN, MSG_COMMAND::INSERT);
    QVariantMap data{{"barcode", mItem.barcode}, {"count", ui->doubleCount->value()}, {"price", (mItem.price - mItem.discount)},
                    {"suplier_id", ui->comboSuplier->currentData()}, {"purchase_item_id", mItem.id},
                    {"total", (mItem.price - mItem.discount) * ui->doubleCount->value()}, {"note", ui->plainNote->toPlainText()},
                    {"name", mItem.name}};
    if(mId <= 0) {
        msg.setData(data);
        msg.addData("user_id", UserSession::id());
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
    ui->pushSaveAgain->setEnabled(false);
}

void ReturnAddDialog::calculateTotal()
{
    ui->labelTotal->setText(Preference::toString((mItem.price - mItem.discount) * ui->doubleCount->value()));
}
