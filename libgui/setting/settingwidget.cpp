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

using namespace LibG;
using namespace LibGUI;
using namespace LibPrint;

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    connect(ui->checkSign, SIGNAL(toggled(bool)), SLOT(signChanged()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    setupAppliaction();
    setupLocale();
    setupPrinter();
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setupAppliaction()
{
    ui->lineAppName->setText(Preference::getString(SETTING::MARKET_NAME, "Sultan Minimarket"));
    ui->plainSubName->setPlainText(Preference::getString(SETTING::MARKET_SUBNAME, "Jl. Bantul\nYogyakarta"));
}

void SettingWidget::setupLocale()
{
    ui->comboApplicationLanguage->addItem("English", "EN");
    ui->comboApplicationLanguage->addItem("Bahasa Indonesia", "ID");
    //TODO : init this with QLocale::Language and QLocale::Country
    ui->comboLocale->addItem("English", QLocale::English);
    ui->comboLocale->addItem("Indonesian", QLocale::Indonesian);
    ui->comboLocaleCounty->addItem("Indonesia", QLocale::Indonesia);
    //Fill
    setCurrentCombo(ui->comboApplicationLanguage, Preference::getString(SETTING::APPLICATION_LANGUAGE, "EN"));
    setCurrentCombo(ui->comboLocale, Preference::getInt(SETTING::LOCALE_LANGUAGE, QLocale::Indonesian));
    setCurrentCombo(ui->comboLocaleCounty, Preference::getInt(SETTING::LOCALE_COUNTRY, QLocale::Indonesia));
    ui->checkSign->setChecked(Preference::getBool(SETTING::LOCALE_USE_SIGN));
    ui->lineSign->setText(Preference::getString(SETTING::LOCALE_SIGN));
    ui->lineSign->setEnabled(ui->checkSign->isChecked());
    ui->spinMoneyDecimal->setValue(Preference::getInt(SETTING::LOCALE_DECIMAL));
}

void SettingWidget::setupPrinter()
{
    ui->comboPrintCashierType->addItem(tr("Device Printer"), PRINT_TYPE::DEVICE);
    ui->comboPrintCashierType->addItem(tr("Spool Printer"), PRINT_TYPE::SPOOL);
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
    //locale
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
    Preference::sync();
    Preference::applyApplicationSetting();
}
