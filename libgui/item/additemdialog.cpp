/*
 * additemdialog.cpp
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
#include "additemdialog.h"
#include "ui_additemdialog.h"
#include "guiutil.h"
#include "message.h"
#include "global_constant.h"
#include "preference.h"
#include "global_setting_const.h"
#include "flashmessagemanager.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddItemDialog::AddItemDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    setMessageBus(bus);
    ui->setupUi(this);
    ui->doubleBuyPrice->setDecimals(Preference::getInt(SETTING::LOCALE_DECIMAL));
    ui->doubleSellPrice->setDecimals(Preference::getInt(SETTING::LOCALE_DECIMAL));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->pushSaveAgain, SIGNAL(clicked(bool)), SLOT(saveAndAgainClicked()));
    connect(ui->lineBarcode, SIGNAL(editingFinished()), SLOT(barcodeDone()));
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::reset(bool isAddAgain)
{
    mIsUpdate = false;
    ui->lineBarcode->clear();
    ui->lineName->clear();
    if(!isAddAgain) {
        ui->comboCategory->setCurrentIndex(0);
        ui->comboSuplier->setCurrentIndex(0);
    }
    ui->doubleBuyPrice->setValue(0);
    ui->doubleSellPrice->setValue(0);
    ui->doubleStock->setValue(0);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->lineBarcode->setEnabled(true);
    ui->pushSave->setEnabled(true);
    ui->pushSaveAgain->setEnabled(true);
    ui->pushSaveAgain->show();
    ui->labelError->hide();
    setWindowTitle(tr("Add New Item"));
}

void AddItemDialog::fill(const QVariantMap &data)
{
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineName->setText(data["name"].toString());
    ui->doubleBuyPrice->setValue(data["buy_price"].toDouble());
    ui->doubleSellPrice->setValue(data["sell_price"].toDouble());
    ui->doubleStock->setValue(data["stock"].toDouble());
    ui->lineBarcode->setEnabled(false);
    GuiUtil::selectCombo(ui->comboCategory, data["category_id"].toInt());
    GuiUtil::selectCombo(ui->comboSuplier, data["suplier_id"].toInt());
    ui->labelError->hide();
    mCurrentSuplier = data["suplier_id"].toInt();
    mCUrrentCategory = data["category_id"].toInt();
}

void AddItemDialog::setAsUpdate()
{
    mIsUpdate = true;
    ui->pushSaveAgain->hide();
    setWindowTitle(tr("Update Item"));
    ui->pushSave->setEnabled(true);
}

void AddItemDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            ui->labelError->setText(tr("Items with barcode already exist : %1").arg(msg->data("name").toString()));
            ui->labelError->show();
        } else {
            ui->labelError->hide();
        }
    } else if(msg->isType(MSG_TYPE::ITEM)) {
        if(msg->isSuccess()) {
            if(mIsUpdate) FlashMessageManager::showMessage(tr("Item updated successfully"));
            else FlashMessageManager::showMessage(tr("Item added successfully"));
            if(!mIsAddAgain) {
                hide();
            } else {
                reset(true);
            }
        } else {
            QMessageBox::warning(this, tr("Error"), msg->data("error").toString());
        }
    } else if(msg->isType(MSG_TYPE::SUPLIER)) {
        const QVariantList &list = msg->data("data").toList();
        populateSuplier(list);
    } else if(msg->isType(MSG_TYPE::CATEGORY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCategory(ui->comboCategory, list, mCUrrentCategory);
    }
}

void AddItemDialog::showEvent(QShowEvent *event)
{
    //reload category and suplier
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
    Message msg2(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    msg2.setLimit(-1);
    sendMessage(&msg2);
    QDialog::showEvent(event);
}

void AddItemDialog::saveData()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineBarcode << ui->lineName << ui->comboCategory << ui->comboSuplier)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill required field"));
        return;
    }
    QVariantMap data;
    data["name"] = ui->lineName->text();
    data["category_id"] = ui->comboCategory->currentData().toInt();
    data["suplier_id"] = ui->comboSuplier->currentData().toInt();
    data["sell_price"] = ui->doubleSellPrice->value();
    data["buy_price"] = ui->doubleBuyPrice->value();
    data["stock"] = ui->doubleStock->value();
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::INSERT);
    if(mIsUpdate) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("barcode", ui->lineBarcode->text());
        msg.addData("data", data);
    } else {
        data["barcode"] = ui->lineBarcode->text();
        msg.setData(data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
    ui->pushSaveAgain->setEnabled(false);
}

void AddItemDialog::populateSuplier(const QVariantList &list)
{
    ui->comboSuplier->clear();
    ui->comboSuplier->addItem(tr("-- Select Suplier --"), 0);
    for(auto &d : list) {
        const QVariantMap &m = d.toMap();
        ui->comboSuplier->addItem(m["name"].toString(), m["id"].toInt());
    }
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
}

void AddItemDialog::barcodeDone()
{
    if(ui->lineBarcode->text().isEmpty()) return;
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", ui->lineBarcode->text());
    sendMessage(&msg);
}

void AddItemDialog::saveClicked()
{
    mIsAddAgain = false;
    saveData();
}

void AddItemDialog::saveAndAgainClicked()
{
    mIsAddAgain = true;
    saveData();
}
