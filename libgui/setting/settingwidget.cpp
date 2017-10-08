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
#include "printtestdialog.h"
#include "message.h"
#include <QMetaEnum>
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
    ui->pushPrintTest->hide();
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged()));
    connect(ui->pushPrintTest, SIGNAL(clicked(bool)), SLOT(printTestClicked()));
    connect(ui->comboLocale, SIGNAL(currentIndexChanged(int)), SLOT(localeLanguageChanged()));
    ui->tabWidget->setCurrentIndex(0);
    Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::QUERY);
    sendMessage(&msg);
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
    ui->comboPrintCashierType->addItem(tr("Device Printer"), PRINT_TYPE::DEVICE);
#ifndef NO_PRINTER_SPOOL
    ui->comboPrintCashierType->addItem(tr("Spool Printer"), PRINT_TYPE::SPOOL);
#endif
    ui->comboPrintCashier->addItems(Printer::instance()->getPrintList());
    connect(ui->comboPrintCashierType, SIGNAL(currentIndexChanged(int)), SLOT(cashierPrintTypeChanged()));
    cashierPrintTypeChanged();
    GuiUtil::selectCombo(ui->comboPrintCashierType, Preference::getInt(SETTING::PRINTER_CASHIER_TYPE));
    ui->comboPrintCashier->setCurrentText(Preference::getString(SETTING::PRINTER_CASHIER_NAME));
    ui->linePrintCashierDevice->setText(Preference::getString(SETTING::PRINTER_CASHIER_DEVICE));
    ui->linePrintCashierTitle->setText(Preference::getString(SETTING::PRINTER_CASHIER_TITLE, "Sultan Minimarket"));
    ui->linePrintCashierSubtitle->setText(Preference::getString(SETTING::PRINTER_CASHIER_SUBTITLE, "Jogonalan Lor RT 2 Bantul"));
    ui->plainPrintCashierFooter->setPlainText(Preference::getString(SETTING::PRINTER_CASHIER_FOOTER, "Barang dibeli tidak dapat ditukar"));
    ui->spinCashierCpi10->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32));
    ui->spinCashierCpi12->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40));
    ui->checkPrintCashierCut->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT));
    ui->checkPrintCashierDrawer->setChecked(Preference::getBool(SETTING::PRINTER_CASHIER_KICK));
    ui->spinCashierLinefeed->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    ui->spinCashierPriceLinefeed->setValue(Preference::getInt(SETTING::PRINTER_CASHIER_PRICE_LINEFEED, 2));
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

void SettingWidget::signChanged()
{
    ui->lineSign->setEnabled(ui->checkSign->isChecked());
}

void SettingWidget::cashierPrintTypeChanged()
{
    ui->comboPrintCashier->setEnabled(ui->comboPrintCashierType->currentData().toInt() == PRINT_TYPE::SPOOL);
    ui->linePrintCashierDevice->setDisabled(ui->comboPrintCashierType->currentData().toInt() == PRINT_TYPE::SPOOL);
}

void SettingWidget::saveClicked()
{
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
    Preference::setValue(SETTING::PRINTER_CASHIER_SUBTITLE, ui->linePrintCashierSubtitle->text());
    Preference::setValue(SETTING::PRINTER_CASHIER_FOOTER, ui->plainPrintCashierFooter->toPlainText());
    Preference::setValue(SETTING::PRINTER_CASHIER_CPI10, ui->spinCashierCpi10->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_CPI12, ui->spinCashierCpi12->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_AUTOCUT, ui->checkPrintCashierCut->isChecked());
    Preference::setValue(SETTING::PRINTER_CASHIER_KICK, ui->checkPrintCashierDrawer->isChecked());
    Preference::setValue(SETTING::PRINTER_CASHIER_LINEFEED, ui->spinCashierLinefeed->value());
    Preference::setValue(SETTING::PRINTER_CASHIER_PRICE_LINEFEED, ui->spinCashierPriceLinefeed->value());
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
    LibPrint::PrintTestDialog dialog(this);
    dialog.exec();
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
    }
}
