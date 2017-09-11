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
#include "util.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "db_constant.h"
#include "addpricedialog.h"
#include "rowdata.h"
#include "keyevent.h"
#include "cashier/searchitemdialog.h"
#include <cmath>
#include <QMessageBox>
#include <QDebug>

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
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(returnPressed()));
    connect(ui->checkPurchase, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkSell, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkIngridient, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkPackage, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkEditPrice, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->groupMultiPrice, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->doubleBuyPrice, SIGNAL(valueChanged(double)), SLOT(calculateDiscount()));
    connect(ui->doubleSellPrice, SIGNAL(valueChanged(double)), SLOT(calculateDiscount()));
    connect(ui->lineDiscountFormula, SIGNAL(textEdited(QString)), SLOT(calculateDiscount()));
    connect(ui->pushPackageItem, SIGNAL(clicked(bool)), SLOT(openSearchItem()));
    connect(ui->doublePackageQty, SIGNAL(valueChanged(double)), SLOT(updatePackagePrice()));
    ui->checkStock->setChecked(true);
    ui->checkSell->setChecked(true);
    ui->checkPurchase->setChecked(true);
    ui->checkNote->setChecked(false);
    ui->checkEditPrice->setChecked(false);
    ui->checkNote->setChecked(false);
    ui->checkPackage->setChecked(false);
    ui->checkProduct->setChecked(false);
    ui->tabWidget->setCurrentIndex(0);
    //table multiprice
    ui->tablePrice->initCrudButton();
    auto model = ui->tablePrice->getModel();
    model->setMessageBus(mMessageBus);
    model->addColumnMoney("count", tr("Qty"));
    model->addColumnMoney("price", tr("Sell Price"));
    model->addColumn("discount_formula", tr("Discount\nFormula"));
    model->addColumnMoney("discount", tr("Discount"));
    model->addColumnMoney("final", tr("Final\nPrice"));
    model->setTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::QUERY);
    model->setAsLocal(true);
    ui->tablePrice->setupTable();
    GuiUtil::setColumnWidth(ui->tablePrice->getTableView(), QList<int>() << 50 << 100 << 100 << 100 << 100);
    connect(ui->tablePrice, SIGNAL(addClicked()), SLOT(addPriceClicked()));
    connect(ui->tablePrice, SIGNAL(updateClicked(QModelIndex)), SLOT(updatePriceClicked(QModelIndex)));
    connect(ui->tablePrice, SIGNAL(deleteClicked(QModelIndex)), SLOT(deletePriceClicked(QModelIndex)));
    ui->toolBoxPrice->setCurrentIndex(0);
    //this is temporary
    ui->checkNote->setEnabled(false);
    ui->checkIngridient->setEnabled(false);
    ui->checkProduct->setEnabled(false);
    ui->checkPackage->setEnabled(false);
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
        ui->comboUnit->setCurrentIndex(0);
        mCurrentSuplier = 0;
        mCUrrentCategory = 0;
        mCurrentUnit = "";
    }
    ui->doubleBuyPrice->setValue(0);
    ui->doubleSellPrice->setValue(0);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->lineBarcode->setEnabled(true);
    ui->pushSave->setEnabled(true);
    ui->pushSaveAgain->setEnabled(true);
    ui->pushSaveAgain->show();
    ui->labelError->hide();
    setWindowTitle(tr("Add New Item"));
    mIsOk = false;
    ui->toolBoxPrice->setCurrentIndex(0);
    ui->tablePrice->getModel()->reset();
    ui->linePackageItem->clear();
    ui->labelPackageName->clear();
    ui->labelPackagePrice->setText("0");
    ui->doublePackageQty->setValue(1);
    ui->doubleStock->setValue(0);
    applyItemFlagToCheckbox(ITEM_FLAG::CALCULATE_STOCK | ITEM_FLAG::PURCHASE | ITEM_FLAG::SELLABLE);
    ui->doubleStock->setEnabled(true);
}

void AddItemDialog::fill(const QVariantMap &data)
{
    int flag = data["flag"].toInt();
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineName->setText(data["name"].toString());
    ui->doubleBuyPrice->setValue(data["buy_price"].toDouble());
    ui->doubleSellPrice->setValue(data["sell_price"].toDouble());
    ui->lineBarcode->setEnabled(false);
    GuiUtil::selectCombo(ui->comboCategory, data["category_id"].toInt());
    GuiUtil::selectCombo(ui->comboSuplier, data["suplier_id"].toInt());
    ui->labelError->hide();
    mCurrentSuplier = data["suplier_id"].toInt();
    mCUrrentCategory = data["category_id"].toInt();
    mCurrentUnit = data["unit"].toString();
    applyItemFlagToCheckbox(flag);
    mIsOk = true;
    ui->tablePrice->getModel()->setAsLocal(false);
    ui->tablePrice->getModel()->setFilter("barcode", COMPARE::EQUAL, data["barcode"].toString());
    ui->tablePrice->getModel()->refresh();
    ui->toolBoxPrice->setCurrentIndex(((flag & ITEM_FLAG::MULTIPRICE) == 0) ? 0 : 1);
    ui->doubleStock->setValue(data["stock"].toFloat());
}

void AddItemDialog::setAsUpdate()
{
    mIsUpdate = true;
    ui->pushSaveAgain->hide();
    setWindowTitle(tr("Update Item"));
    ui->pushSave->setEnabled(true);
    ui->doubleStock->setEnabled(false);
}

void AddItemDialog::disableAddAgain()
{
    ui->pushSaveAgain->hide();
}

void AddItemDialog::setBarcode(const QString &barcode)
{
    ui->lineBarcode->setText(barcode);
    barcodeDone();
    ui->lineName->setFocus(Qt::TabFocusReason);
}

void AddItemDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            ui->labelError->setText(tr("Items with barcode already exist : %1").arg(msg->data("name").toString()));
            ui->labelError->show();
            ui->lineBarcode->setFocus();
            ui->lineBarcode->selectAll();
        } else {
            ui->labelError->hide();
            mIsOk = true;
            if(mIsReturnPressed) ui->lineName->setFocus(Qt::TabFocusReason);
        }
        mIsReturnPressed = false;
    } else if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::UPDATE) || msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            if(mIsUpdate) FlashMessageManager::showMessage(tr("Item updated successfully"));
            else FlashMessageManager::showMessage(tr("Item added successfully"));
            mIsSuccess = true;
            emit success();
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
        GuiUtil::populateCombo(ui->comboSuplier, list, tr("-- Select Suplier --"));
        GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
    } else if(msg->isType(MSG_TYPE::CATEGORY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCategory, list, tr("-- Select Category --"));
        GuiUtil::selectCombo(ui->comboCategory, mCUrrentCategory);
    } else if(msg->isType(MSG_TYPE::UNIT)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboUnit, list, tr("-- Select Unit --"));
        GuiUtil::selectComboByText(ui->comboUnit, mCurrentUnit);
    } else if(msg->isTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::INSERT)) {
        FlashMessageManager::showMessage(tr("Price inserted successfully"));
        ui->tablePrice->getModel()->refresh();
    } else if(msg->isTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::UPDATE)) {
        FlashMessageManager::showMessage(tr("Price updated successfully"));
        ui->tablePrice->getModel()->refresh();
    } else if(msg->isTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::DEL)) {
        FlashMessageManager::showMessage(tr("Price deleted successfully"));
        ui->tablePrice->getModel()->refresh();
    } else if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE)) {
        mPriceList = msg->data("prices").toList();
        updatePackagePrice();
    }
}

void AddItemDialog::showEvent(QShowEvent *event)
{
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
    Message msg2(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    msg2.setLimit(-1);
    sendMessage(&msg2);
    Message msg3(MSG_TYPE::UNIT, MSG_COMMAND::QUERY);
    msg2.setLimit(-1);
    sendMessage(&msg3);
    QDialog::showEvent(event);
}

void AddItemDialog::saveData()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineBarcode << ui->lineName << ui->comboCategory << ui->comboSuplier)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill required field"));
        return;
    }
    QVariantMap data;
    int flag = getItemFlagFromCheckbox();
    data["name"] = ui->lineName->text();
    data["category_id"] = ui->comboCategory->currentData().toInt();
    data["suplier_id"] = ui->comboSuplier->currentData().toInt();
    data["sell_price"] = ui->doubleSellPrice->value();
    data["buy_price"] = ui->doubleBuyPrice->value();
    data["unit"] = ui->comboUnit->currentText();
    data["flag"] = flag;
    if((flag & ITEM_FLAG::MULTIPRICE) == 0) {
        double disc = Util::calculateDiscount(ui->lineDiscountFormula->text(), ui->doubleSellPrice->value());
        QVariantMap d{{"barcode", ui->lineBarcode->text()}, {"count", 1}, {"price", ui->doubleSellPrice->value()},
                     {"discount_formula", ui->lineDiscountFormula->text()}, {"discount", disc}, {"final", ui->doubleSellPrice->value() - disc}};
        data["sell_price"] = d;
    } else {
        auto rdata = ui->tablePrice->getModel()->getRowData();
        QVariantList prices;
        for(int i = 0; i < rdata->size(); i++) {
            prices << rdata->at(i)->data();
        }
        data["sell_price"] = prices;
    }
    if((flag & ITEM_FLAG::PACKAGE) == 0) {
        data["barcode_link"] = "";
        data["count_link"] = 0;
    } else {
        data["barcode_link"] = ui->linePackageItem->text();
        data["count_link"] = ui->doublePackageQty->value();
        if(ui->linePackageItem->text().isEmpty() || ui->doublePackageQty->value() == 0) {
            QMessageBox::critical(this, tr("Error"), tr("Box package not correctly filled"));
            return;
        }
    }
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::INSERT);
    if(mIsUpdate) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("barcode", ui->lineBarcode->text());
        msg.addData("data", data);
    } else {
        data["stock"] = ui->doubleStock->value();
        data["barcode"] = ui->lineBarcode->text();
        msg.setData(data);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
    ui->pushSaveAgain->setEnabled(false);
}

int AddItemDialog::getItemFlagFromCheckbox()
{
    int flag = 0;
    flag |= (ui->groupMultiPrice->isChecked() ? ITEM_FLAG::MULTIPRICE : 0);
    flag |= (ui->checkEditPrice->isChecked() ? ITEM_FLAG::EDITABLE_PRICE : 0);
    flag |= (ui->checkNote->isChecked() ? ITEM_FLAG::REQUIRE_NOTE : 0);
    flag |= (ui->checkStock->isChecked() ? ITEM_FLAG::CALCULATE_STOCK : 0);
    flag |= (ui->checkPurchase->isChecked() ? ITEM_FLAG::PURCHASE : 0);
    flag |= (ui->checkIngridient->isChecked() ? ITEM_FLAG::HAS_INGRIDIENT : 0);
    flag |= (ui->checkSell->isChecked() ? ITEM_FLAG::SELLABLE : 0);
    flag |= (ui->checkProduct->isChecked() ? ITEM_FLAG::PROCESS_FIRST : 0);
    flag |= (ui->checkPackage->isChecked() ? ITEM_FLAG::PACKAGE : 0);
    return flag;
}

void AddItemDialog::applyItemFlagToCheckbox(int flag)
{
    ui->groupMultiPrice->setChecked((flag & ITEM_FLAG::MULTIPRICE) != 0);
    ui->checkEditPrice->setChecked((flag & ITEM_FLAG::EDITABLE_PRICE) != 0);
    ui->checkNote->setChecked((flag & ITEM_FLAG::REQUIRE_NOTE) != 0);
    ui->checkStock->setChecked((flag & ITEM_FLAG::CALCULATE_STOCK) != 0);
    ui->checkPurchase->setChecked((flag & ITEM_FLAG::PURCHASE) != 0);
    ui->checkIngridient->setChecked((flag & ITEM_FLAG::HAS_INGRIDIENT) != 0);
    ui->checkSell->setChecked((flag & ITEM_FLAG::SELLABLE) != 0);
    ui->checkProduct->setChecked((flag & ITEM_FLAG::PROCESS_FIRST) != 0);
    ui->checkPackage->setChecked((flag & ITEM_FLAG::PACKAGE) != 0);
}

void AddItemDialog::barcodeDone()
{
    if(ui->lineBarcode->text().isEmpty()) return;
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", ui->lineBarcode->text());
    sendMessage(&msg);
    mIsOk = false;
}

void AddItemDialog::returnPressed()
{
    mIsReturnPressed = true;
    barcodeDone();
}

void AddItemDialog::saveClicked()
{
    if(!mIsOk) return;
    mIsAddAgain = false;
    saveData();
}

void AddItemDialog::saveAndAgainClicked()
{
    if(!mIsOk) return;
    mIsAddAgain = true;
    saveData();
}

void AddItemDialog::checkWidget()
{
    auto sender = qobject_cast<QCheckBox*>(QObject::sender());
    ui->checkEditPrice->setEnabled(ui->checkSell->isChecked());
    /*ui->checkIngridient->setEnabled(ui->checkSell->isChecked());
    ui->checkNote->setEnabled(ui->checkSell->isChecked());*/
    if(sender == ui->checkSell) {
        ui->checkEditPrice->setChecked(false);
        /*ui->checkNote->setChecked(false);
        ui->checkIngridient->setChecked(false);*/
    }
    if(sender == ui->checkPackage && !ui->checkPackage->isChecked())
        ui->tabWidget->setCurrentIndex(Price);
    if(sender == ui->checkPackage) {
        if(ui->checkPackage->isChecked())
            ui->groupMultiPrice->setChecked(false);
        ui->groupMultiPrice->setEnabled(!ui->checkPackage->isChecked());
    }
    if(sender == ui->checkEditPrice) {
        ui->groupMultiPrice->setChecked(false);
        ui->groupMultiPrice->setEnabled(!ui->checkEditPrice->isChecked());
    }

    ui->tabWidget->setTabEnabled(Ingridient, ui->checkIngridient->isChecked());
    ui->tabWidget->setTabEnabled(Package, ui->checkPackage->isChecked());
    ui->groupSinglePrice->setEnabled(!ui->groupMultiPrice->isChecked());
}

void AddItemDialog::calculateDiscount()
{
    double disc = Util::calculateDiscount(ui->lineDiscountFormula->text(), ui->doubleSellPrice->value());
    double margin = ui->doubleSellPrice->value() - disc - ui->doubleBuyPrice->value();
    double perc = 100;
    if(ui->doubleBuyPrice->value() > 0) perc = margin * 100 / ui->doubleBuyPrice->value();
    ui->labelPriceDiscount->setText(Preference::toString(disc));
    ui->labelFinalPrice->setText(Preference::toString(ui->doubleSellPrice->value() - disc));
    ui->labelPriceMargin->setText(QString("%1 (%2%)").arg(Preference::toString(margin)).arg(Preference::toString(perc)));
}

void AddItemDialog::addPriceClicked()
{
    AddPriceDialog dialog(ui->tablePrice->getModel()->isLocal(), this);
    dialog.reset();
    dialog.setBuyPrice(ui->doubleBuyPrice->value());
    dialog.exec();
    if(!dialog.isOk()) return;
    QVariantMap data = dialog.getData();
    auto rdata = ui->tablePrice->getModel()->getRowData();
    for(int i = 0; i < rdata->size(); i++) {
        float qty = rdata->at(i)->data("count").toFloat();
        if(qty == data["count"].toFloat()) {
            QMessageBox::critical(this, tr("Error"), tr("Quantity already exist"));
            return;
        }
    }
    if(ui->tablePrice->getModel()->isLocal()) {
        auto item = new TableItem();
        item->fill(data);
        ui->tablePrice->getModel()->appendItem(item);
    } else {
        Message msg(MSG_TYPE::SELLPRICE, MSG_COMMAND::INSERT);
        data["barcode"] = ui->lineBarcode->text();
        msg.setData(data);
        sendMessage(&msg);
    }
}

void AddItemDialog::updatePriceClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    AddPriceDialog dialog(ui->tablePrice->getModel()->isLocal(), this);
    dialog.reset();
    dialog.fill(item->data());
    dialog.setBuyPrice(ui->doubleBuyPrice->value());
    dialog.exec();
    if(!dialog.isOk()) return;
    const QVariantMap &data = dialog.getData();
    auto rdata = ui->tablePrice->getModel()->getRowData();
    for(int i = 0; i < rdata->size(); i++) {
        if(rdata->at(i) == item) continue;
        float qty = rdata->at(i)->data("count").toFloat();
        if(qty == data["count"].toFloat()) {
            QMessageBox::critical(this, tr("Error"), tr("Quantity already exist"));
            return;
        }
    }
    if(ui->tablePrice->getModel()->isLocal()) {
        item->fill(data);
        ui->tablePrice->getModel()->refresh();
        FlashMessageManager::showMessage(tr("Price updated successfully"));
    } else {
        Message msg(MSG_TYPE::SELLPRICE, MSG_COMMAND::UPDATE);
        msg.addData("id", item->id);
        msg.addData("data", data);
        sendMessage(&msg);
    }
}

void AddItemDialog::deletePriceClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    int res = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure to delete the price?"));
    if(res == QMessageBox::Yes) {
        if(ui->tablePrice->getModel()->isLocal()) {
            ui->tablePrice->getModel()->removeItem(item);
            FlashMessageManager::showMessage(tr("Price deleted successfully"));
        } else {
            Message msg(MSG_TYPE::SELLPRICE, MSG_COMMAND::DEL);
            msg.addData("id", item->id);
            sendMessage(&msg);
        }
    }
}

void AddItemDialog::openSearchItem()
{
    SearchItemDialog dialog(mMessageBus, true, this);
    dialog.exec();
    if(dialog.isOk()) {
        const QVariantMap &data = dialog.getSelectedData();
        if(ui->lineBarcode->text().compare(data["barcode"].toString()) == 0) {
            QMessageBox::critical(this, tr("Error"), tr("Package can not consist its own"));
            return;
        }
        ui->linePackageItem->setText(data["barcode"].toString());
        ui->labelPackageName->setText(data["name"].toString());
        getItemPrice();
    }
}

void AddItemDialog::getItemPrice()
{
    LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE);
    msg.addData("barcode", ui->linePackageItem->text());
    sendMessage(&msg);
}

double AddItemDialog::updatePackagePrice()
{
    double total = 0;
    float count = ui->doublePackageQty->value();
    for(int i = mPriceList.count() - 1; i >= 0; i--) {
        const QVariantMap &p = mPriceList[i].toMap();
        const double &price = p["price"].toDouble();
        const float &c = p["count"].toFloat();
        const QString &discformula = p["discount_formula"].toString();
        if(c <= count) {
            if(i == 0) {
                total += count * (price - Util::calculateDiscount(discformula, price));
            } else {
                float temp = std::fmod(count, c);
                total += (count - temp) * (price - Util::calculateDiscount(discformula, price));
                count = count - temp;
                if(count <= 0.0f) break;
            }
        }
    }
    ui->labelPackagePrice->setText(Preference::toString(total));
    return total;
}
