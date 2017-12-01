/*
 * transactionlistdialog.cpp
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
#include "transactionlistdialog.h"
#include "ui_transactionlistdialog.h"
#include "tableitem.h"
#include "tableview.h"
#include "guiutil.h"
#include "tablemodel.h"
#include "dbutil.h"
#include "global_constant.h"
#include "message.h"
#include "db_constant.h"
#include "keyevent.h"
#include "solditemlistdialog.h"
#include <QShortcut>

using namespace LibGUI;
using namespace LibG;

TransactionListDialog::TransactionListDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransactionListDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto model = ui->tableWidget->getModel();
    auto dateFormater = [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    };
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date"), Qt::AlignLeft, dateFormater);
    model->addColumn("number", tr("Number"));
    model->addColumnMoney("total", tr("Total"));
    model->setTypeCommand(MSG_TYPE::SOLD, MSG_COMMAND::QUERY);
    model->setSort("id desc");
    ui->tableWidget->setupTable();
    GuiUtil::setColumnWidth(ui->tableWidget->getTableView(), QList<int>() << 150 << 200 << 150);
    ui->tableWidget->getTableView()->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->getTableView()->setUseStandardHeader(true);
    model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(focusAndSelectTable()));
    new QShortcut(QKeySequence(Qt::Key_P), this, SLOT(printTransaction()));
    connect(ui->lineNumber, SIGNAL(returnPressed()), SLOT(search()));
    auto key = new KeyEvent(ui->tableWidget->getTableView());
    key->addConsumeKey(Qt::Key_Return);
    ui->tableWidget->getTableView()->installEventFilter(key);
    connect(key, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(showItems()));
    connect(ui->tableWidget->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(showItems()));
}

TransactionListDialog::~TransactionListDialog()
{
    delete ui;
}

void TransactionListDialog::setType(int type)
{
    if(type == SoldReturn) {
        ui->groupBox->hide();
    }
    mType = type;
}

void TransactionListDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::SOLD, MSG_COMMAND::GET) && msg->isSuccess()) {
        mPrintFunction(msg->data());
    }
}

void TransactionListDialog::focusAndSelectTable()
{
    ui->tableWidget->getTableView()->setFocus(Qt::TabFocusReason);
    if(ui->tableWidget->getModel()->rowCount(QModelIndex()) > 0)
        ui->tableWidget->getTableView()->selectRow(0);
}

void TransactionListDialog::printTransaction()
{
    const auto &index = ui->tableWidget->getTableView()->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    Message msg(MSG_TYPE::SOLD, MSG_COMMAND::GET);
    msg.addData("id", item->id);
    sendMessage(&msg);
}

void TransactionListDialog::search()
{
    ui->tableWidget->getModel()->setFilter("number", COMPARE::LIKE, ui->lineNumber->text());
    ui->tableWidget->getModel()->refresh();
}

void TransactionListDialog::showItems()
{
    const QModelIndex &index = ui->tableWidget->getTableView()->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    SoldItemListDialog dialog(item->data(), mMessageBus, this);
    dialog.exec();
    if(mType == SoldReturn && dialog.isOk()) {
        mIsOk = true;
        mData = dialog.getData();
        mData["sold_id"] = item->data("id");
        mData["sold_created_at"] = item->data("created_at");
        mData["sold_number"] = item->data("number");
        mIsOk = true;
        close();
    }
}
