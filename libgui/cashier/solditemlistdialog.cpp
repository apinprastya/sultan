/*
 * solditemlistdialog.cpp
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
#include "solditemlistdialog.h"
#include "ui_solditemlistdialog.h"
#include "tableitem.h"
#include "tableview.h"
#include "guiutil.h"
#include "tablemodel.h"
#include "dbutil.h"
#include "global_constant.h"
#include "message.h"
#include "db_constant.h"
#include "keyevent.h"
#include "cashieritem.h"
#include "tableitem.h"
#include "flashmessagemanager.h"
#include "returnitemadddialog.h"

using namespace LibGUI;
using namespace LibG;

SoldItemListDialog::SoldItemListDialog(const QVariantMap &data, LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoldItemListDialog),
    mId(data["id"].toInt())
{
    ui->setupUi(this);
    ui->labelNumber->setText(data["number"].toString());
    ui->labelDate->setText(LibDB::DBUtil::sqlDateToDateTime(data["created_at"].toString()).toString("dd-MM-yyyy hh:mm"));
    auto model = ui->tableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumnMoney("count", tr("Count"));
    model->addColumnMoney("price", tr("Price"));
    model->addColumnMoney("discount", tr("Discount"));
    model->addColumnMoney("total", tr("Total"));
    model->setFilter("sold_id", COMPARE::EQUAL, data["id"]);
    model->setFilter("flag", COMPARE::FLAG_DISABLE, ITEM_FLAG::ITEM_LINK);
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::QUERY);
    model->setSort("id asc");
    ui->tableWidget->setupTable();
    GuiUtil::setColumnWidth(ui->tableWidget->getTableView(), QList<int>() << 100 << 200 << 80 << 100 << 100 << 100);
    ui->tableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->getTableView()->setUseStandardHeader(true);
    model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(focusAndSelectTable()));
    auto keyevent = new KeyEvent(this);
    keyevent->addConsumeKey(Qt::Key_Return);
    ui->tableWidget->getTableView()->installEventFilter(keyevent);
    connect(keyevent, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(tableReturnPressed()));
    connect(ui->tableWidget->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(tableReturnPressed()));
}

SoldItemListDialog::~SoldItemListDialog()
{
    delete ui;
}

void SoldItemListDialog::messageReceived(LibG::Message */*msg*/)
{
}

void SoldItemListDialog::focusAndSelectTable()
{
    ui->tableWidget->getTableView()->setFocus(Qt::TabFocusReason);
    if(ui->tableWidget->getModel()->rowCount(QModelIndex()) > 0)
        ui->tableWidget->getTableView()->selectRow(0);
}

void SoldItemListDialog::tableReturnPressed()
{
    const QModelIndex &index = ui->tableWidget->getTableView()->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    mData = item->data();
    mIsOk = true;
    close();
    /*int flag = item->data("flag").toInt();
    if((flag & CashierItem::Return) != 0) {
        FlashMessageManager::showError(tr("Return item can not return again"));
        return;
    } else if ((flag & CashierItem::Returned) != 0) {
        FlashMessageManager::showError(tr("Item already returned"));
        return;
    }
    ReturnItemAddDialog dialog(this);
    dialog.fill(item->data());
    dialog.exec();*/
}
