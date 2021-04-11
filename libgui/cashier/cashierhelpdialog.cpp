/*
 * cashierhelpdialog.cpp
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
#include "cashierhelpdialog.h"
#include "ui_cashierhelpdialog.h"
#include <QFile>

using namespace LibGUI;

CashierHelpDialog::CashierHelpDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CashierHelpDialog) {
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Key")));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Detail")));
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    addRow("F1", tr("Open this help"));
    addRow("F2", tr("Search items"));
    addRow("F3", tr("Scan customer"));
    addRow("Shift + F3", tr("Search customer"));
    addRow("Ctrl + F3", tr("Reset customer"));
    addRow("F4", tr("Pay cash transaction"));
    addRow("F5", tr("Open drawer"));
    addRow("F6", tr("History transaction list"));
    addRow("F7", tr("Check Prices"));
    addRow("F8", tr("Non full customer payment"));
    addRow("F9", tr("Cashless payment"));
    addRow("F10", tr("Add non stock transaction"));
    addRow("F12", tr("Cashier Report"));
    addRow("Ctrl + N", tr("Clear current cart and start new transaction"));
    addRow("Ctrl + Del", tr("Clear current cart and start new transaction"));
    addRow("Ctrl + P", tr("Customer credit payment"));
    addRow("Ctrl + Arrow Up", tr("Focus to table"));
    addRow("Ctrl + Arrow Down", tr("Focus to barcode input"));
    addRow("Ctrl + T", tr("Open new cashier session"));
    addRow("Ctrl + PgUp", tr("Go to previous cashier session"));
    addRow("Ctrl + PgDn", tr("Go to next cashier session"));
    addRow("Ctrl + Plus", tr("Add auto item"));
    addRow("PgDn", tr("Edit last inserted item"));
}

CashierHelpDialog::~CashierHelpDialog() { delete ui; }

void CashierHelpDialog::addRow(const QString &key, const QString &detail) {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row + 1);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(key));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(detail));
}
