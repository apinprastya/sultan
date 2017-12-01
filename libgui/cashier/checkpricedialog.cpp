/*
 * checkpricedialog.cpp
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
#include "checkpricedialog.h"
#include "ui_checkpricedialog.h"
#include "messagebus.h"
#include "tablemodel.h"
#include "tableview.h"
#include "global_constant.h"
#include "guiutil.h"
#include "tableitem.h"
#include "db_constant.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

CheckPriceDialog::CheckPriceDialog(LibG::MessageBus *bus, const QString &barcode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckPriceDialog)
{
    ui->setupUi(this);
    auto model = ui->tableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumnMoney("count", tr("Count"));
    model->addColumnMoney("price", tr("Sell Price"));
    model->addColumn("discount_formula", tr("Discount Formula"));
    model->addColumnMoney("discount", tr("Discount"));
    model->addColumnMoney("final", tr("Price - Discount"));
    model->setTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::QUERY);
    model->setSort("id desc");
    ui->tableWidget->setupTable();
    GuiUtil::setColumnWidth(ui->tableWidget->getTableView(), QList<int>() << 100 << 150 << 150 << 150);
    ui->tableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->getTableView()->setUseStandardHeader(true);
    connect(model, SIGNAL(firstDataLoaded()), SLOT(focusAndSelectTable()));
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(search()));
    if(!barcode.isEmpty()) {
        ui->lineBarcode->setText(barcode);
        ui->lineBarcode->selectAll();
        search();
    }
}

CheckPriceDialog::~CheckPriceDialog()
{
    delete ui;
}

void CheckPriceDialog::search()
{
    auto model = ui->tableWidget->getModel();
    model->setFilter("barcode", COMPARE::EQUAL, ui->lineBarcode->text());
    model->refresh();
}

void CheckPriceDialog::focusAndSelectTable()
{
    if(ui->tableWidget->getModel()->rowCount(QModelIndex()) == 0) {
        ui->labelName->setText(tr("Item not found"));
        QMessageBox::critical(this, tr("Error"), tr("Item not found"));
        return;
    }
    const QModelIndex &index = ui->tableWidget->getModel()->index(0, 0);
    auto item = static_cast<TableItem*>(index.internalPointer());
    ui->labelName->setText(item->data("name").toString());
    ui->tableWidget->getTableView()->selectRow(0);
    ui->tableWidget->setFocus(Qt::TabFocusReason);
}
