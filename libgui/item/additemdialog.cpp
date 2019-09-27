/*
 * additemdialog.cpp
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
#include "restoreitemdialog.h"
#include "keyevent.h"
#include "addingridientdialog.h"
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
    //connect(ui->lineBarcode, SIGNAL(editingFinished()), SLOT(barcodeDone()));
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
    connect(ui->pushSaveClose, SIGNAL(clicked(bool)), SLOT(saveCloseClicked()));
    auto ke = new KeyEvent(this);
    ke->addConsumeKey(Qt::Key_Tab);
    ui->lineBarcode->installEventFilter(ke);
    connect(ke, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(returnPressed()));
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
    ui->tablePrice->getTableView()->setSelectionMode(QAbstractItemView::SingleSelection);
    GuiUtil::setColumnWidth(ui->tablePrice->getTableView(), QList<int>() << 50 << 100 << 100 << 100 << 100);
    connect(ui->tablePrice, SIGNAL(addClicked()), SLOT(addPriceClicked()));
    connect(ui->tablePrice, SIGNAL(updateClicked(QModelIndex)), SLOT(updatePriceClicked(QModelIndex)));
    connect(ui->tablePrice, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deletePriceClicked(QModelIndexList)));
    ui->toolBoxPrice->setCurrentIndex(0);
    //this is temporary
    ui->checkProduct->setEnabled(false);
    //item links table
    model = ui->tableItemLink->getModel();
    model->setMessageBus(mMessageBus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->setTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::QUERY);
    GuiUtil::setColumnWidth(ui->tablePrice->getTableView(), QList<int>() << 150 << 100);
    ui->tableItemLink->getTableView()->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tableItemLink->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(tableItemLinkDoubleClicked()));
    //ingridient table
    model = ui->tableIngridient->getModel();
    model->setMessageBus(mMessageBus);
    model->addColumn("barcode_link", tr("Barcode"));
    model->addColumn("name_link", tr("Name"));
    model->addColumn("count_link", tr("Count"), Qt::AlignRight);
    model->addColumnMoney("buy_price", tr("Buy Price"));
    model->addColumnMoney("sell_price", tr("Sell Price"));
    model->setTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::QUERY);
    ui->tableIngridient->getTableView()->setUseStandardHeader(true);
    ui->tableIngridient->initCrudButton();
    ui->tableIngridient->getTableView()->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableIngridient, SIGNAL(addClicked()), SLOT(addIngridient()));
    connect(ui->tableIngridient, SIGNAL(updateClicked(QModelIndex)), SLOT(updateIngridient(QModelIndex)));
    connect(ui->tableIngridient, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteIngridient(QModelIndexList)));
    GuiUtil::setColumnWidth(ui->tablePrice->getTableView(), QList<int>() << 150 << 100);
    ui->tableItemLink->getTableView()->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tableItemLink->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(tableItemLinkDoubleClicked()));
    ui->pushSaveClose->hide();
    connect(ui->tableIngridient->getModel(), SIGNAL(firstDataLoaded()), SLOT(calculateIngridientPrice()));
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
    ui->lineBarcode->setEnabled(true);
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    ui->pushSaveAgain->setEnabled(true);
    ui->pushSaveAgain->show();
    ui->pushSaveClose->hide();
    ui->labelError->hide();
    setWindowTitle(tr("Add New Item"));
    mIsOk = false;
    mIsFromSave = false;
    ui->toolBoxPrice->setCurrentIndex(0);
    ui->tablePrice->getModel()->reset();
    ui->tablePrice->getModel()->setAsLocal(true);
    ui->tableIngridient->getModel()->reset();
    ui->tableIngridient->getModel()->setAsLocal(true);
    ui->linePackageItem->clear();
    ui->labelPackageName->clear();
    ui->labelPackagePrice->setText("0");
    ui->doublePackageQty->setValue(1);
    ui->doubleStock->setValue(0);
    applyItemFlagToCheckbox(ITEM_FLAG::CALCULATE_STOCK | ITEM_FLAG::PURCHASE | ITEM_FLAG::SELLABLE);
    ui->doubleStock->setEnabled(true);
    ui->tabWidget->setTabEnabled(ItemLink, false);
    ui->labelWarningItemLink->hide();
    ui->checkStock->setChecked(true);
    ui->checkSell->setChecked(true);
    ui->checkPurchase->setChecked(true);
    ui->checkNote->setChecked(false);
    ui->checkEditPrice->setChecked(false);
    ui->checkNote->setChecked(false);
    ui->checkPackage->setChecked(false);
    ui->checkProduct->setChecked(false);
    ui->labelIngridientPrice->setText("0");
    ui->labelIngridientSellPrice->setText("0");
}

void AddItemDialog::openBarcode(const QString &barcode)
{
    mCurrentBarcode = barcode;
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::GET);
    msg.addData("barcode", barcode);
    sendMessage(&msg);
    Message msg2(MSG_TYPE::ITEMLINK, MSG_COMMAND::QUERY);
    msg2.addFilter("barcode", COMPARE::EQUAL, barcode);
    mBarcodeLinkRequest = sendMessage(&msg2);
    Message msg3(MSG_TYPE::ITEMLINK, MSG_COMMAND::QUERY);
    msg3.addFilter("barcode_link", COMPARE::EQUAL, barcode);
    mBarcodeOtherLinkRequest = sendMessage(&msg3);
}

void AddItemDialog::fill(const QVariantMap &data)
{
    int flag = data["flag"].toInt();
    applyItemFlagToCheckbox(flag);
    ui->lineBarcode->setText(data["barcode"].toString());
    ui->lineName->setText(data["name"].toString());
    if((flag & ITEM_FLAG::PACKAGE) == 0)
        ui->doubleBuyPrice->setValue(data["buy_price"].toDouble());
    ui->doubleSellPrice->setValue(data["sell_price"].toDouble());
    ui->lineBarcode->setEnabled(false);
    GuiUtil::selectCombo(ui->comboCategory, data["category_id"].toInt());
    GuiUtil::selectCombo(ui->comboSuplier, data["suplier_id"].toInt());
    GuiUtil::selectComboByText(ui->comboUnit, data["unit"].toString());
    ui->labelError->hide();
    mCurrentSuplier = data["suplier_id"].toInt();
    mCUrrentCategory = data["category_id"].toInt();
    mCurrentUnit = data["unit"].toString();
    mIsOk = true;
    ui->tablePrice->getModel()->setAsLocal(false);
    ui->tablePrice->getModel()->setFilter("barcode", COMPARE::EQUAL, data["barcode"].toString());
    ui->tablePrice->getModel()->refresh();
    ui->toolBoxPrice->setCurrentIndex(((flag & ITEM_FLAG::MULTIPRICE) == 0) ? 0 : 1);
    ui->doubleStock->setValue(data["stock"].toFloat());
    ui->tableIngridient->getModel()->setAsLocal(false);
    ui->tableIngridient->getModel()->setFilter("barcode", COMPARE::EQUAL, data["barcode"].toString());
    ui->tableIngridient->getModel()->refresh();
}

void AddItemDialog::setAsUpdate()
{
    mIsUpdate = true;
    ui->pushSaveAgain->hide();
    setWindowTitle(tr("Update Item"));
    ui->pushSave->setEnabled(true);
    ui->pushSaveClose->show();
    ui->doubleStock->setEnabled(false);
}

void AddItemDialog::disableAddAgain()
{
    ui->pushSaveAgain->hide();
}

void AddItemDialog::setBarcode(const QString &barcode)
{
    ui->lineBarcode->setText(barcode);
    mCurrentBarcode = barcode;
    barcodeDone();
    ui->lineName->setFocus(Qt::TabFocusReason);
}

void AddItemDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            if(mIsUpdate) {
                fill(msg->data());
            } else {
                if(!msg->data("deleted_at").toString().isEmpty()) {
                    RestoreItemDialog dialog(msg->data(), this);
                    dialog.exec();
                    if(dialog.isOk()) {
                        Message message(MSG_TYPE::ITEM, MSG_COMMAND::RESTORE_DELETED);
                        message.addData("barcode", msg->data("barcode"));
                        sendMessage(&message);
                    }
                } else {
                    ui->labelError->setText(tr("Items with barcode already exist : %1").arg(msg->data("name").toString()));
                    ui->labelError->show();
                    ui->lineBarcode->setFocus();
                    ui->lineBarcode->selectAll();
                }
            }
        } else {
            ui->labelError->hide();
            mIsOk = true;
            if(mIsReturnPressed) ui->lineName->setFocus(Qt::TabFocusReason);
            if(mIsFromSave) saveClicked();
        }
        mIsReturnPressed = false;
    } else if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::UPDATE) || msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            if(mIsUpdate) FlashMessageManager::showMessage(tr("Item updated successfully"));
            else FlashMessageManager::showMessage(tr("Item added successfully"));
            mIsSuccess = true;
            emit success();
            if(!mIsAddAgain) {
                if(mIsCloseAfter || !mIsUpdate) hide();
                ui->pushSave->setEnabled(true);
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
        const QVariantMap &it = msg->data("item").toMap();
        mPriceList = msg->data("prices").toList();
        ui->labelPackageName->setText(it["name"].toString());
        mPackBuyPrice = it["buy_price"].toDouble();
        updatePackagePrice();
    } else if(msg->isTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::QUERY)) {
        const QVariantList &l = msg->data("data").toList();
        if(mBarcodeLinkRequest == msg->getUniqueId()) {
            if(l.size() > 0) {
                const QVariantMap &m = l[0].toMap();
                ui->linePackageItem->setText(m["barcode_link"].toString());
                ui->doublePackageQty->setValue(m["count_link"].toFloat());
                getItemPrice();
            }
        } else if(mBarcodeOtherLinkRequest == msg->getUniqueId()) {
            if(l.size() > 0) {
                ui->tabWidget->setTabEnabled(ItemLink, true);
                ui->labelWarningItemLink->show();
                ui->tableItemLink->getModel()->setFilter("barcode_link", COMPARE::EQUAL, mCurrentBarcode);
                ui->tableItemLink->getModel()->refresh();
            }
        }
    } else if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::RESTORE_DELETED)) {
        if(msg->isSuccess()) {
            reset();
            setAsUpdate();
            openBarcode(msg->data("barcode").toString());
        }
    } else if(msg->isTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::UPDATE)) {
        FlashMessageManager::showMessage(tr("Ingridient item updated successfully"));
        ui->tableIngridient->getModel()->refresh();
    } else if(msg->isTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::INSERT)) {
        FlashMessageManager::showMessage(tr("Ingridient item inserted successfully"));
        ui->tableIngridient->getModel()->refresh();
    } else if(msg->isTypeCommand(MSG_TYPE::ITEMLINK, MSG_COMMAND::DEL)) {
        FlashMessageManager::showMessage(tr("Ingridient item removed successfully"));
        ui->tableIngridient->getModel()->refresh();
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
    data["name"] = Util::capitalize(ui->lineName->text().trimmed().replace("\n", ""));
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
    if((flag & ITEM_FLAG::PACKAGE) != 0) {
        QVariantMap linkdata;
        linkdata["barcode_link"] = ui->linePackageItem->text();
        linkdata["count_link"] = ui->doublePackageQty->value();
        if(ui->linePackageItem->text().isEmpty() || ui->doublePackageQty->value() == 0) {
            QMessageBox::critical(this, tr("Error"), tr("Box package not correctly filled"));
            return;
        }
        data["box"] = linkdata;
    }
    if((flag & ITEM_FLAG::HAS_INGRIDIENT) != 0) {
        auto rdata = ui->tableIngridient->getModel()->getRowData();
        QVariantList indrigients;
        for(int i = 0; i < rdata->size(); i++) {
            indrigients << rdata->at(i)->data();
        }
        data["ingridients"] = indrigients;
    }
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::INSERT);
    if(mIsUpdate) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("barcode", ui->lineBarcode->text());
        data.remove("ingridients");
        msg.addData("data", data);
    } else {
        data["stock"] = ui->doubleStock->value();
        data["barcode"] = ui->lineBarcode->text().trimmed().replace("\n", "");
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
    msg.addData("withdeleted", true);
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
    if(!mIsOk) {
        mIsFromSave = true;
        returnPressed();
        return;
    }
    mIsCloseAfter = false;
    mIsAddAgain = false;
    saveData();
}

void AddItemDialog::saveAndAgainClicked()
{
    if(!mIsOk) {
        mIsFromSave = true;
        returnPressed();
        return;
    }
    mIsAddAgain = true;
    mIsCloseAfter = false;
    saveData();
}

void AddItemDialog::saveCloseClicked()
{
    if(!mIsOk) {
        mIsFromSave = true;
        returnPressed();
        return;
    }
    mIsAddAgain = false;
    mIsCloseAfter = true;
    saveData();
}

void AddItemDialog::checkWidget()
{
    auto sender = qobject_cast<QCheckBox*>(QObject::sender());
    ui->checkEditPrice->setEnabled(ui->checkSell->isChecked());
    ui->checkIngridient->setEnabled(ui->checkSell->isChecked() && !ui->checkPackage->isChecked());
    ui->checkPackage->setEnabled(ui->checkSell->isChecked() && !ui->checkIngridient->isChecked());
    ui->checkPurchase->setEnabled(!ui->checkPackage->isChecked() && !ui->checkIngridient->isChecked());
    ui->doubleBuyPrice->setEnabled(!ui->checkPackage->isChecked() && !ui->checkIngridient->isChecked());
    ui->doubleStock->setEnabled(!mIsUpdate && !ui->checkPackage->isChecked() && !ui->checkIngridient->isChecked());
    if(sender == ui->checkSell) {
        ui->checkEditPrice->setChecked(false);
        ui->checkIngridient->setChecked(false);
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
    if(!ui->checkPurchase->isEnabled()) ui->checkPurchase->setChecked(false);

    ui->tabWidget->setTabEnabled(Ingridient, ui->checkIngridient->isChecked());
    ui->tabWidget->setTabEnabled(Package, ui->checkPackage->isChecked());
    ui->groupSinglePrice->setEnabled(!ui->groupMultiPrice->isChecked());
    ui->pushPackageItem->setEnabled(!mIsUpdate);
    ui->doublePackageQty->setEnabled(!mIsUpdate);
}

void AddItemDialog::calculateDiscount()
{
    double disc = Util::calculateDiscount(ui->lineDiscountFormula->text(), ui->doubleSellPrice->value());
    double margin = ui->doubleSellPrice->value() - disc - ui->doubleBuyPrice->value();
    double perc = 100;
    if(ui->doubleBuyPrice->value() > 0) perc = margin * 100 / ui->doubleBuyPrice->value();
    ui->labelPriceDiscount->setText(Preference::formatMoney(disc));
    ui->labelFinalPrice->setText(Preference::formatMoney(ui->doubleSellPrice->value() - disc));
    ui->labelPriceMargin->setText(QString("%1 (%2%)").arg(Preference::formatMoney(margin)).arg(Preference::formatMoney(perc)));
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

void AddItemDialog::deletePriceClicked(const QModelIndexList &index)
{
    if(index.isEmpty()) return;
    auto item = static_cast<TableItem*>(index[0].internalPointer());
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
        //ui->labelPackageName->setText(data["name"].toString());
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
    if(!ui->checkPackage->isChecked()) return 0;
    double total = 0;
    float count = ui->doublePackageQty->value();
    ui->doubleBuyPrice->setValue(mPackBuyPrice * count);
    for(int i = mPriceList.count() - 1; i >= 0; i--) {
        const QVariantMap &p = mPriceList[i].toMap();
        const double &price = p["price"].toDouble();
        const float &c = p["count"].toFloat();
        const QString &discformula = p["discount_formula"].toString();
        if(c <= count || i == 0) {
            if(i == 0) {
                total += count * (price - Util::calculateDiscount(discformula, price));
            } else {
                float temp = std::fmod(count, c);
                total += (count - temp) * (price - Util::calculateDiscount(discformula, price));
                count -= (count - temp);
                if(count <= 0.0f) break;
            }
        }
    }
    ui->labelPackagePrice->setText(Preference::formatMoney(total));
    return total;
}

void AddItemDialog::tableItemLinkDoubleClicked()
{
    const QModelIndex &i = ui->tableItemLink->getTableView()->currentIndex();
    if(i.isValid()) {
        auto item = static_cast<TableItem*>(i.internalPointer());
        AddItemDialog dialog(mMessageBus, this);
        dialog.reset();
        dialog.setAsUpdate();
        dialog.openBarcode(item->data("barcode").toString());
        dialog.exec();
    }
}

void AddItemDialog::addIngridient()
{
    AddIngridientDialog dialog(mMessageBus, this);
    dialog.exec();
    if(!dialog.isOk()) return;
    QVariantMap data = dialog.getData();
    auto rdata = ui->tableIngridient->getModel()->getRowData();
    for(int i = 0; i < rdata->size(); i++) {
        if(!rdata->at(i)->data("barcode_link").toString().compare(data["barcode"].toString())) {
            QMessageBox::critical(this, tr("Error"), tr("Item with barcode already exist"));
            return;
        }
    }
    if(ui->tableIngridient->getModel()->isLocal()) {
        auto item = new TableItem();
        float count = data["count_link"].toFloat();
        item->fill(QVariantMap{{"barcode_link", data["barcode_link"]}, {"name_link", data["name_link"]},
                               {"count_link", count}, {"buy_price", data["buy_price"].toDouble()},
                               {"sell_price", data["sell_final"].toDouble()}});
        ui->tableIngridient->getModel()->appendItem(item);
        calculateIngridientPrice();
    } else {
        Message msg(MSG_TYPE::ITEMLINK, MSG_COMMAND::INSERT);
        msg.setData(QVariantMap{{"barcode", ui->lineBarcode->text()}, {"type", ITEM_LINK_TYPE::INGRIDIENT},
                    {"barcode_link", data["barcode_link"]}, {"count_link", data["count_link"]}});
        sendMessage(&msg);
    }
}

void AddItemDialog::updateIngridient(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    AddIngridientDialog dialog(mMessageBus, this);
    dialog.setData(item->data());
    dialog.exec();
    if(!dialog.isOk()) return;
    const QVariantMap &data = dialog.getData();
    if(ui->tableIngridient->getModel()->isLocal()) {
        item->fill(data);
        ui->tableIngridient->getModel()->refresh();
        FlashMessageManager::showMessage(tr("Ingridient updated successfully"));
        calculateIngridientPrice();
    } else {
        Message msg(MSG_TYPE::ITEMLINK, MSG_COMMAND::UPDATE);
        msg.addData("id", item->id);
        msg.addData("data", QVariantMap{{"count_link", data["count_link"]}});
        sendMessage(&msg);
    }
}

void AddItemDialog::deleteIngridient(const QModelIndexList &index)
{
    if(index.empty()) return;
    auto item = static_cast<TableItem*>(index[0].internalPointer());
    int res = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure to delete the ingridient?"));
    if(res == QMessageBox::Yes) {
        if(ui->tableIngridient->getModel()->isLocal()) {
            ui->tableIngridient->getModel()->removeItem(item);
            FlashMessageManager::showMessage(tr("Ingridient deleted successfully"));
            calculateIngridientPrice();
        } else {
            Message msg(MSG_TYPE::ITEMLINK, MSG_COMMAND::DEL);
            msg.addData("id", item->id);
            sendMessage(&msg);
        }
    }
}

void AddItemDialog::calculateIngridientPrice()
{
    if(!ui->checkIngridient->isChecked()) return;
    auto rdata = ui->tableIngridient->getModel()->getRowData();
    double buyPrice = 0;
    double sellPrice = 0;
    for(int i = 0; i < rdata->size(); i++) {
        float count = rdata->at(i)->data("count_link").toDouble();
        buyPrice += rdata->at(i)->data("buy_price").toDouble() * count;
        sellPrice += rdata->at(i)->data("sell_price").toDouble() * count;
    }
    ui->labelIngridientPrice->setText(Preference::formatMoney(buyPrice));
    ui->labelIngridientSellPrice->setText(Preference::formatMoney(sellPrice));
    ui->doubleBuyPrice->setValue(buyPrice);
}
