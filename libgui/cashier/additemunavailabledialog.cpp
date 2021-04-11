/*
 * additemunavailabledialog.cpp
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

#include "additemunavailabledialog.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "guiutil.h"
#include "message.h"
#include "preference.h"
#include "ui_additemunavailabledialog.h"
#include "util.h"

#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddItemUnavailableDialog::AddItemUnavailableDialog(MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddItemUnavailableDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->doublePrice, SIGNAL(valueChanged(double)), SLOT(sellPriceChanged()));
}

AddItemUnavailableDialog::~AddItemUnavailableDialog() { delete ui; }

void AddItemUnavailableDialog::openBarcode(const QString &barcode) {
    mIsAutoBarcode = false;
    fill(barcode);
}

void AddItemUnavailableDialog::openAutoBarcode() {
    mIsAutoBarcode = true;
    fill("AUTO");
}

void AddItemUnavailableDialog::fill(const QString &barcode) {
    ui->lineBarcode->setText(barcode);
    ui->lineBarcode->setReadOnly(true);
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->lineName->clear();
    ui->doublePrice->setValue(0.0f);
    ui->pushSave->setEnabled(true);
    ui->comboSuplier->setEnabled(!Preference::getBool(SETTING::CAI_SUPLIER));
    ui->comboCategory->setEnabled(!Preference::getBool(SETTING::CAI_CATEGORY));
    ui->comboUnit->setEnabled(!Preference::getBool(SETTING::CAI_UNIT));
    ui->doubleBuyPrice->setEnabled(!Preference::getBool(SETTING::CAI_MARGIN));
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    sendMessage(&msg);
    Message msg1(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    sendMessage(&msg1);
    Message msg2(MSG_TYPE::UNIT, MSG_COMMAND::QUERY);
    sendMessage(&msg2);
    show();
}

void AddItemUnavailableDialog::messageReceived(Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboSuplier, list, tr("-- Select Suplier --"));
        if (Preference::getBool(SETTING::CAI_SUPLIER))
            GuiUtil::selectCombo(ui->comboSuplier, Preference::getInt(SETTING::CAI_DEFAULT_SUPLIER));
    } else if (msg->isTypeCommand(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCategory, list, tr("-- Select Category --"));
        if (Preference::getBool(SETTING::CAI_CATEGORY))
            GuiUtil::selectCombo(ui->comboCategory, Preference::getInt(SETTING::CAI_DEFAULT_CATEGORY));
    } else if (msg->isTypeCommand(MSG_TYPE::UNIT, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboUnit, list, tr("-- Select Unit --"));
        if (Preference::getBool(SETTING::CAI_UNIT))
            GuiUtil::selectCombo(ui->comboUnit, Preference::getInt(SETTING::CAI_DEFAULT_UNIT));
    }
}

void AddItemUnavailableDialog::saveClicked() {
    if (ui->lineName->text().isEmpty()) {
        ui->lineName->setFocus(Qt::TabFocusReason);
        QMessageBox::critical(this, tr("Error"), tr("Name must be filled"));
        return;
    }
    if (ui->comboSuplier->currentData().toInt() <= 0 || ui->comboCategory->currentData().toInt() <= 0 ||
        ui->comboUnit->currentData().toInt() <= 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select correct suplier, category, and unit"));
        return;
    }
    if (ui->doublePrice->value() <= 0 || ui->doubleBuyPrice->value() <= 0) {
        ui->doublePrice->setFocus(Qt::TabFocusReason);
        QMessageBox::critical(this, tr("Error"), tr("Price and buy price must greater than 0"));
        return;
    }
    int flag = ITEM_FLAG::CALCULATE_STOCK | ITEM_FLAG::PURCHASE | ITEM_FLAG::SELLABLE;
    QVariantMap d{{"barcode", ui->lineBarcode->text()},
                  {"count", 1},
                  {"price", ui->doublePrice->value()},
                  {"discount_formula", ""},
                  {"discount", 0},
                  {"final", ui->doublePrice->value()}};
    emit addNewItem(QVariantMap{{"barcode", ui->lineBarcode->text()},
                                {"name", Util::capitalize(ui->lineName->text())},
                                {"category_id", ui->comboCategory->currentData()},
                                {"suplier_id", ui->comboSuplier->currentData()},
                                {"flag", flag},
                                {"unit", ui->comboUnit->currentText()},
                                {"buy_price", ui->doubleBuyPrice->value()},
                                {"sell_price", d},
                                {"autobarcode", mIsAutoBarcode}});
    ui->pushSave->setEnabled(false);
}

void AddItemUnavailableDialog::sellPriceChanged() {
    if (Preference::getBool(SETTING::CAI_MARGIN)) {
        double margin = Preference::getDouble(SETTING::CAI_DEFAULT_MARGIN) / 100;
        double buy = ui->doublePrice->value() / (1 + margin);
        ui->doubleBuyPrice->setValue(buy);
    }
}
