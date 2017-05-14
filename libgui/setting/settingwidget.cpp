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

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    setupLocale();
    setupPrinter();
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setupLocale()
{
    ui->comboApplicationLanguage->addItem("English", "EN");
    ui->comboApplicationLanguage->addItem("Bahasa Indonesia", "ID");
    //TODO : init this with QLocale::Language and QLocale::Country
    ui->comboLocale->addItem("English", QLocale::English);
    ui->comboLocale->addItem("Indonesian", QLocale::Indonesian);
    ui->comboLocaleCounty->addItem("Indonesia", QLocale::Indonesia);
}

void SettingWidget::setupPrinter()
{

}
