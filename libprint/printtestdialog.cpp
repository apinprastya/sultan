/*
 * printtestdialog.cpp
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
#include "printtestdialog.h"
#include "ui_printtestdialog.h"
#include "printer.h"
#include "escp.h"
#include "global_setting_const.h"
#include "global_constant.h"
#include "preference.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibPrint;
using namespace LibG;

PrintTestDialog::PrintTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintTestDialog)
{
    ui->setupUi(this);
    QStringList printers = Printer::instance()->getPrintList();
    ui->comboPrint->addItems(printers);
    connect(ui->pushPrint, SIGNAL(clicked(bool)), SLOT(printClicked()));
    ui->comboType->addItem(QLatin1String("Device"), PRINT_TYPE::DEVICE);
    ui->comboType->addItem(QLatin1String("Spool"), PRINT_TYPE::SPOOL);
    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(updateEnable()));
    connect(ui->pushOpenDrawer, SIGNAL(clicked(bool)), SLOT(openDrawer()));
    updateEnable();
}

PrintTestDialog::~PrintTestDialog()
{
    delete ui;
}

void PrintTestDialog::printClicked()
{
    Escp e(10, 10, 0);
    QString str;
    QString str2;
    QString str3;
    int counter = 1;
    while(counter <= ui->spin10->value()) {
        str.append(QString::number(counter % 10));
        counter++;
    }
    counter = 1;
    while(counter <= ui->spin12->value()) {
        str3.append(QString::number(counter % 10));
        counter++;
    }
    counter = 1;
    while(counter <= ui->spin15->value()) {
        str3.append(QString::number(counter % 10));
        counter++;
    }
    e.cpi10();
    e.leftText(str, true);
    e.newLine();
    e.leftText(QLatin1String("----------"));
    e.newLine();
    e.cpi12();
    e.leftText(str2, true);
    e.newLine();
    e.leftText(QLatin1String("----------"));
    e.newLine();
    e.cpi15();
    e.leftText(str3, true);
    e.newLine();
    e.leftText(QLatin1String("----------"));
    e.newLine(3);
    int type = ui->comboType->currentData().toInt();
    QString printName = type == PRINT_TYPE::DEVICE ? ui->lineDevice->text() : ui->comboPrint->currentText();
    if(printName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please specify the printer"));
        return;
    }
    if(ui->checkDrawer->isChecked()) Printer::instance()->print(printName, Escp::openDrawerCommand(), type);
    Printer::instance()->print(printName, e.data(), type);
    if(ui->checkCut->isChecked()) Printer::instance()->print(printName, Escp::cutPaperCommand());
}

void PrintTestDialog::updateEnable()
{
    if(ui->comboType->currentIndex() == 0) {
        ui->comboPrint->setEnabled(false);
        ui->lineDevice->setEnabled(true);
    } else {
        ui->comboPrint->setEnabled(true);
        ui->lineDevice->setEnabled(false);
    }
}

void PrintTestDialog::openDrawer()
{
    int type = ui->comboType->currentData().toInt();
    QString printName = type == PRINT_TYPE::DEVICE ? ui->lineDevice->text() : ui->comboPrint->currentText();
    if(printName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please specify the printer"));
        return;
    }
    Printer::instance()->print(printName, Escp::openDrawerCommand(), type);
}
