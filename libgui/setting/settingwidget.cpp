/*
 * settingwidget.cpp
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
#include "settingwidget.h"
#include "ui_settingwidget.h"
#include "preference.h"
#include "global_setting_const.h"
#include "global_constant.h"
#include "printer.h"
#include "guiutil.h"
#include "message.h"
#include "guiutil.h"
#include "escp.h"
#include <QMetaEnum>
#include <QMessageBox>
#include <QDebug>

#define TAB_APPLICATION     0
#define TAB_LOCALE          1
#define TAB_PRINT           2

using namespace LibG;
using namespace LibGUI;
using namespace LibPrint;

SettingWidget::SettingWidget(MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    setMessageBus(bus);
    connect(ui->checkSign, SIGNAL(toggled(bool)), SLOT(signChanged()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    setupAppliaction();
    setupLocale();
    setupPrinter();
    setupCashier();
    ui->pushPrintTest->hide();
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged()));
    connect(ui->pushPrintTest, SIGNAL(clicked(bool)), SLOT(printTestClicked()));
    connect(ui->comboLocale, SIGNAL(currentIndexChanged(int)), SLOT(localeLanguageChanged()));
    ui->tabWidget->setCurrentIndex(0);
    Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::QUERY);
    sendMessage(&msg);
    Message msg2(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    sendMessage(&msg2);
    Message msg3(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    sendMessage(&msg3);
    Message msg4(MSG_TYPE::UNIT, MSG_COMMAND::QUERY);
    sendMessage(&msg4);
    Message msg5(MSG_TYPE::CONFIG, MSG_COMMAND::QUERY);
    sendMessage(&msg5);
    checkWidget();
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setupAppliaction()
{
    ui->lineAppName->setText(Preference::getString(SETTING::MARKET_NAME, "Sultan Minimarket"));
    ui->plainSubName->setPlainText(Preference::getString(SETTING::MARKET_SUBNAME, "Jl. Bantul\nYogyakarta"));

    ui->groupBoxTax->setChecked(Preference::getBool(SETTING::USE_TAX));
    ui->lineSalesTax->setText(Preference::getString(SETTING::TAX_VALUE));

    ui->groupNameBased->setChecked(Preference::getBool(SETTING::CASHIER_NAMEBASED));
    ui->groupCapitalize->setChecked(Preference::getBool(SETTING::CAPITALIZE));
    ui->groupUseMinimumOrder->setChecked(Preference::getBool(SETTING::MULTIPLE_MINIMUM));
}

void SettingWidget::setupLocale()
{
    ui->comboApplicationLanguage->addItem("English", "en");
    ui->comboApplicationLanguage->addItem("Bahasa Indonesia", "id");
    GuiUtil::selectCombo(ui->comboApplicationLanguage, Preference::getString(SETTING::APPLICATION_LANGUAGE, "id"));
    QMetaEnum meta = QMetaEnum::fromType<QLocale::Language>();
    mAllLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for(auto locale : mAllLocales)
        mLocaleLanguage.insert(QLatin1String(meta.valueToKey(locale.language())), locale.language());
    QMapIterator<QString, QLocale::Language> i(mLocaleLanguage);
    while (i.hasNext()) {
        i.next();
        ui->comboLocale->addItem(i.key(), i.value());
    }
    //Fill
    setCurrentCombo(ui->comboApplicationLanguage, Preference::getString(SETTING::APPLICATION_LANGUAGE, "EN"));
    setCurrentCombo(ui->comboLocale, Preference::getInt(SETTING::LOCALE_LANGUAGE, QLocale::Indonesian));
    localeLanguageChanged();
    setCurrentCombo(ui->comboLocaleCounty, Preference::getInt(SETTING::LOCALE_COUNTRY, QLocale::Indonesia));
    ui->checkSign->setChecked(Preference::getBool(SETTING::LOCALE_USE_SIGN));
    ui->lineSign->setText(Preference::getString(SETTING::LOCALE_SIGN));
    ui->lineSign->setEnabled(ui->checkSign->isChecked());
    ui->spinMoneyDecimal->setValue(Preference::getInt(SETTING::LOCALE_DECIMAL));
}

void SettingWidget::setupPrinter()
{
#ifndef NO_PRINTER_DEVICE
    ui->comboPrintCashierType->addItem(tr("Device Printer"), PRINT_TYPE::DEVICE);
#else
    ui->linePrintCashierDevice->hide();
    ui->labelPrinterDevice->hide();
#endif
#ifndef NO_PRINTER_SPOOL
    ui->comboPrintCashierType->addItem(tr("Spool Printer"), PRINT_TYPE::SPOOL);
#else
    ui->labelPrinter->hide();
    ui->comboPrintCashier->hide();
#endif
#ifdef USE_LIBUSB
    ui->comboPrintCashierType->addItem(tr("USB Printer"), PRINT_TYPE::USB);
    mUsbDevices = Usb::getUsbPrinterList();
    ui->comboUsb->clear();
    uint16_t vendorId = (uint16_t)Preference::getInt(SETTING::PRINTER_CASHIER_VENDOR_ID);
    uint16_t produkId = (uint16_t)Preference::getInt(SETTING::PRINTER_CASHIER_PRODUK_ID);
    int curSel = 0;
    for(int i = 0; i < mUsbDevices.size(); i++) {
        ui->comboUsb->addItem(mUsbDevices[i].name);
        if(vendorId == mUsbDevices[i].vendorId && produkId == mUsbDevices[i].productId)
            curSel = i;
    }
    ui->comboUsb->setCurrentIndex(curSel);
#else
    ui->comboUsb->hide();
    ui->labelUsb->hide();
#endif
    ui->comboPrintCashier->addItems(Printer::instance()->getPrintList());
    connect(ui->comboPrintCashierType, SIGNAL(currentIndexChanged(int)), SLOT(cashierPrintTypeChanged()));
    cashierPrintTypeChanged();
    GuiUtil::selectCombo(ui->comboPrintCashierType, Preference::getInt(SETTING::PRINTER_CASHIER_TYPE));
    ui->comboPrintCashier->setCurrentText(Preference::getString(SETTING::PRINTER_CASHIER_NAME));
    ui->linePrintCashierDevice->setText(Preference::getString(SETTING::PRINTER_CASHIER_DEVICE));
    ui->linePrintCashierTitle->setText(Preference::getString(SETTING::PRINTER_CASHIER_TITLE, "Sultan Minimarket"));
    ui->plainPrintCashierSubtitle->setPlainText(Preference::getString(SETTING::PRINTER_CASHIER_SUBTITLE, "Jogonalan Lor RT 2 Bantul"));
    ui->plainPrintCashierFooter->setPlainText(Preference::getString(SETTING::PRINTER_CASHIER_FOOTER, "Barang dibeli tidak dapat ditukar"));
    ui->spinCashierCpi10->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32));
    ui->spinCashierCpi12->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40));
    ui->checkPrintCashierCut->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT));
    ui->checkPrintCashierDrawer->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_KICK));
    ui->spinCashierLinefeed->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    ui->spinCashierPriceLinefeed->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_PRICE_LINEFEED, 2));
    ui->spinBarcodeLength->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_BARCODE_LEN, 15));
    ui->checkShowBarcode->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_SHOW_BARCODE));
    ui->checkOnly10->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
}

void SettingWidget::setupCashier()
{
    ui->groupCAI->setChecked(Preference::getBool(SETTING::CAI_ENABLE));
    ui->checkCAISuplier->setChecked(Preference::getBool(SETTING::CAI_SUPLIER));
    ui->checkCAICategory->setChecked(Preference::getBool(SETTING::CAI_CATEGORY));
    ui->checkCAIUnit->setChecked(Preference::getBool(SETTING::CAI_UNIT));
    ui->checkCAIBuyPrice->setChecked(Preference::getBool(SETTING::CAI_MARGIN));
    ui->doubleCAIBuyPrice->setValue(Preference::getDouble(SETTING::CAI_DEFAULT_MARGIN));
    ui->checkInlineEdit->setChecked(Preference::getBool(SETTING::INLINE_EDIT_QTY));
    connect(ui->checkCAISuplier, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkCAICategory, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkCAIUnit, SIGNAL(toggled(bool)), SLOT(checkWidget()));
    connect(ui->checkCAIBuyPrice, SIGNAL(toggled(bool)), SLOT(checkWidget()));
}

void SettingWidget::setCurrentCombo(QComboBox *combo, QVariant value)
{
    for(int i = 0; i < combo->count(); i++) {
        if(combo->itemData(i) == value) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

void SettingWidget::updateFromDBConfig(const QVariantList &data)
{
    for(int i = 0; i < data.size(); i++) {
        const QVariantMap &d = data[i].toMap();
        int id = d["id"].toInt();
        switch(id) {
        case CONFIG_DB::AUTOBARCODE_DIGIT:
            ui->spinCABarcodeLength->setValue(d["value"].toInt());
            break;
        case CONFIG_DB::AUTOBARCODE_PREFIX:
            ui->lineCABarcodePrefix->setText(d["value"].toString());
            break;
        }
    }
}

void SettingWidget::saveToDbConfig()
{
    QVariantList list;
    list.append(QVariantMap{{"id", CONFIG_DB::AUTOBARCODE_DIGIT}, {"value", ui->spinCABarcodeLength->value()}});
    list.append(QVariantMap{{"id", CONFIG_DB::AUTOBARCODE_PREFIX}, {"value", ui->lineCABarcodePrefix->text()}});
    Message msg(MSG_TYPE::CONFIG, MSG_COMMAND::CONFIG_INSERT_UPDATE);
    msg.addData("data", list);
    sendMessage(&msg);
}

void SettingWidget::signChanged()
{
    ui->lineSign->setEnabled(ui->checkSign->isChecked());
}

void SettingWidget::cashierPrintTypeChanged()
{
    ui->comboPrintCashier->setEnabled(ui->comboPrintCashierType->currentData().toInt() == PRINT_TYPE::SPOOL);
    ui->linePrintCashierDevice->setEnabled(ui->comboPrintCashierType->currentData().toInt() == PRINT_TYPE::DEVICE);
    ui->comboUsb->setEnabled(ui->comboPrintCashierType->currentData().toInt() == PRINT_TYPE::USB);
}

void SettingWidget::saveClicked()
{
    saveToDbConfig();
    //market name
    Preference::setValue(SETTING::MARKET_NAME, ui->lineAppName->text());
    Preference::setValue(SETTING::MARKET_SUBNAME, ui->plainSubName->toPlainText());
    Preference::setValue(SETTING::MACHINE_ID, ui->comboMachine->currentData().toMap()["id"]);
    Preference::setValue(SETTING::MACHINE_CODE, ui->comboMachine->currentData().toMap()["code"]);
    Preference::setValue(SETTING::MACHINE_NAME, ui->comboMachine->currentText());
    //taxes
    Preference::setValue(SETTING::USE_TAX, ui->groupBoxTax->isChecked());
    Preference::setValue(SETTING::TAX_VALUE, ui->lineSalesTax->text());
    Preference::setValue(SETTING::CASHIER_NAMEBASED, ui->groupNameBased->isChecked());
    Preference::setValue(SETTING::CAPITALIZE, ui->groupCapitalize->isChecked());
    Preference::setValue(SETTING::MULTIPLE_MINIMUM, ui->groupUseMinimumOrder->isChecked());
    //locale
    Preference::setValue(SETTING::APPLICATION_LANGUAGE, ui->comboApplicationLanguage->currentData());
    Preference::setValue(SETTING::LOCALE_COUNTRY, ui->comboLocaleCounty->currentData().toInt());
    Preference::setValue(SETTING::LOCALE_LANGUAGE, ui->comboLocale->currentData().toInt());
    Preference::setValue(SETTING::LOCALE_USE_SIGN, ui->checkSign->isChecked());
    Preference::setValue(SETTING::LOCALE_SIGN, ui->lineSign->text());
    Preference::setValue(SETTING::LOCALE_DECIMAL, ui->spinMoneyDecimal->value());
    //printer
    Preference::setValue(SETTING::PRINTER_CASHIER_TYPE, ui->comboPrintCashierType->currentData());
    Preference::setValue(SETTING::PRINTER_CASHIER_NAME, ui->comboPrintCashier->currentText());
    Preference::setValue(SETTING::PRINTER_CASHIER_DEVICE, ui->linePrintCashierDevice->text());
    Preference::setValue(SETTING::PRINTER_CASHIER_TITLE, ui->linePrintCashierTitle->text());
    Preference::setValue(SETTING::PRINTER_CASHIER_SUBTITLE, ui->plainPrintCashierSubtitle->toPlainText());
    Preference::setValue(SETTING::PRINTER_CASHIER_FOOTER, ui->plainPrintCashierFooter->toPlainText());
    Preference::setValue(SETTING::PRINTER_CASHIER_CPI10, ui->spinCashierCpi10->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_CPI12, ui->spinCashierCpi12->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_AUTOCUT, ui->checkPrintCashierCut->isChecked());
    Preference::setValue(SETTING::PRINTER_CASHIER_KICK, ui->checkPrintCashierDrawer->isChecked());
    Preference::setValue(SETTING::PRINTER_CASHIER_LINEFEED, ui->spinCashierLinefeed->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_PRICE_LINEFEED, ui->spinCashierPriceLinefeed->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_BARCODE_LEN, ui->spinBarcodeLength->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_SHOW_BARCODE, ui->checkShowBarcode->isChecked());
    Preference::setValue(SETTING::PRINTER_CASHIER_ONLY_CPI10, ui->checkOnly10->isChecked());
    Preference::setValue(SETTING::INLINE_EDIT_QTY, ui->checkInlineEdit->isChecked());
#ifdef USE_LIBUSB
    int cur = ui->comboUsb->currentIndex();
    if(ui->comboUsb->isEnabled() && mUsbDevices.size() > 0) {
        Preference::setValue(SETTING::PRINTER_CASHIER_PRODUK_ID, mUsbDevices[cur].productId);
        Preference::setValue(SETTING::PRINTER_CASHIER_VENDOR_ID, mUsbDevices[cur].vendorId);
    }
#endif
    //cashier
    Preference::setValue(SETTING::CAI_ENABLE, ui->groupCAI->isChecked());
    Preference::setValue(SETTING::CAI_SUPLIER, ui->checkCAISuplier->isChecked());
    Preference::setValue(SETTING::CAI_CATEGORY, ui->checkCAICategory->isChecked());
    Preference::setValue(SETTING::CAI_UNIT, ui->checkCAIUnit->isChecked());
    Preference::setValue(SETTING::CAI_MARGIN, ui->checkCAIBuyPrice->isChecked());
    Preference::setValue(SETTING::CAI_DEFAULT_SUPLIER, ui->comboCAISuplier->currentData());
    Preference::setValue(SETTING::CAI_DEFAULT_CATEGORY, ui->comboCAICategory->currentData());
    Preference::setValue(SETTING::CAI_DEFAULT_UNIT, ui->comboCAIUnit->currentData());
    Preference::setValue(SETTING::CAI_DEFAULT_MARGIN, ui->doubleCAIBuyPrice->value());
    Preference::sync();
    Preference::applyApplicationSetting();
}

void SettingWidget::tabChanged()
{
    if(ui->tabWidget->currentIndex() == TAB_PRINT) ui->pushPrintTest->show();
    else ui->pushPrintTest->hide();
}

void SettingWidget::printTestClicked()
{
    int type = ui->comboPrintCashierType->currentData().toInt();
    Escp e(type, 20, 20);
    QString str;
    QString str2;
    int counter = 1;
    while(counter <= 100) {
        str.append(QString::number(counter % 10));
        counter++;
    }
    counter = 1;
    while(counter <= 100) {
        str2.append(QString::number(counter % 10));
        counter++;
    }
    e.cpi10()->leftText("SULTAN TEST PAGE", true)->newLine()->line();
    e.leftText("CPI 10 :", true)->newLine();
    e.leftText(str, true)->newLine()->line();
    e.cpi12()->leftText("CPI 12 :")->newLine();
    e.leftText(str2, true)->newLine();
    e.line()->newLine(ui->spinCashierLinefeed->value());
    QString printName = type == PRINT_TYPE::DEVICE ? ui->linePrintCashierDevice->text() : PRINT_TYPE::SPOOL ? ui->comboPrintCashier->currentText() : ui->comboUsb->currentText();
    if(printName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please specify the printer"));
        return;
    }
    uint16_t vendorId = 0;
    uint16_t productId = 0;
    if(mUsbDevices.size() > 0) {
        vendorId = mUsbDevices[ui->comboUsb->currentIndex()].vendorId;
        productId = mUsbDevices[ui->comboUsb->currentIndex()].productId;
    }
    if(ui->checkPrintCashierDrawer->isChecked()) Printer::instance()->print(printName, Escp::openDrawerCommand(), type, vendorId, productId);
    Printer::instance()->print(printName, e.data(), type);
    if(ui->checkPrintCashierCut->isChecked()) Printer::instance()->print(printName, Escp::cutPaperCommand(), type, vendorId, productId);
}

void SettingWidget::localeLanguageChanged()
{
    mLocaleCountry.clear();
    ui->comboLocaleCounty->clear();
    QLocale::Language curLang = (QLocale::Language)ui->comboLocale->currentData().toInt();
    QMetaEnum meta = QMetaEnum::fromType<QLocale::Country>();
    for(auto locale : mAllLocales) {
        if(locale.language() == curLang) {
            mLocaleCountry.insert(QLatin1String(meta.valueToKey(locale.country())), locale.country());
        }
    }
    QMapIterator<QString, QLocale::Country> i(mLocaleCountry);
    while (i.hasNext()) {
        i.next();
        ui->comboLocaleCounty->addItem(i.key(), i.value());
    }
}

void SettingWidget::checkWidget()
{
    ui->comboCAISuplier->setEnabled(ui->checkCAISuplier->isChecked());
    ui->comboCAICategory->setEnabled(ui->checkCAICategory->isChecked());
    ui->comboCAIUnit->setEnabled(ui->checkCAIUnit->isChecked());
    ui->doubleCAIBuyPrice->setEnabled(ui->checkCAIBuyPrice->isChecked());
}

void SettingWidget::messageReceived(Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::QUERY)) {
        ui->comboMachine->clear();
        ui->comboMachine->addItem(tr("-- Select Machine --"), 0);
        const QVariantList &list = msg->data("data").toList();
        for(auto d : list) {
            const QVariantMap &data = d.toMap();
            ui->comboMachine->addItem(data["name"].toString(), data);
        }
        GuiUtil::selectCombo(ui->comboMachine, Preference::getInt(SETTING::MACHINE_ID, 0), "id");
    } else if(msg->isTypeCommand(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCAISuplier, list, tr("-- Select Suplier --"));
        GuiUtil::selectCombo(ui->comboCAISuplier, Preference::getInt(SETTING::CAI_DEFAULT_SUPLIER));
    } else if(msg->isTypeCommand(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCAICategory, list, tr("-- Select Category --"));
        GuiUtil::selectCombo(ui->comboCAICategory, Preference::getInt(SETTING::CAI_DEFAULT_CATEGORY));
    } else if(msg->isTypeCommand(MSG_TYPE::UNIT, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(ui->comboCAIUnit, list, tr("-- Select Unit --"));
        GuiUtil::selectCombo(ui->comboCAIUnit, Preference::getInt(SETTING::CAI_DEFAULT_UNIT));
    } else if(msg->isTypeCommand(MSG_TYPE::CONFIG, MSG_COMMAND::QUERY)) {
        const QVariantList &list = msg->data("data").toList();
        updateFromDBConfig(list);
    } else if(msg->isTypeCommand(MSG_TYPE::CONFIG, MSG_COMMAND::CONFIG_INSERT_UPDATE)) {
        Message msg2(MSG_TYPE::BROADCAST, MSG_BROADCAST::SETTING_CHANGES);
        sendMessage(&msg2);
    }
}
