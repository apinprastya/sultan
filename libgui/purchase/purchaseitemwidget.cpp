/*
 * purchaseitemwidget.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "purchaseitemwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "global_constant.h"
#include "guiutil.h"
#include "db_constant.h"
#include "message.h"
#include "purchaseadditemdialog.h"
#include "headerwidget.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchaseItemWidget::PurchaseItemWidget(int id, const QString &number, LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mId(id),
    mTableWidget(new TableWidget(this)),
    mAddDialog(new PurchaseAddItemDialog(bus, id, this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Purchase Item : %1").arg(number));
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("count", tr("Count"));
    model->addColumnMoney("price", tr("price"));
    model->addColumnMoney("total", tr("total"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::GET);
    model->setFilter("purchase_id", COMPARE::EQUAL, id);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 200 << 150 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    model->refresh();
    ui->verticalLayout->addWidget(mTableWidget);
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndex)), SLOT(delClicked(QModelIndex)));
    connect(mAddDialog, SIGNAL(addSuccess()), mTableWidget->getModel(), SLOT(refresh()));
    connect(mAddDialog, SIGNAL(updateSuccess(QVariant)), model, SLOT(resfreshOne(QVariant)));
}

PurchaseItemWidget::~PurchaseItemWidget()
{
    delete ui;
}

void PurchaseItemWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::DEL)) {
        if(msg->isSuccess()) {
            mTableWidget->getModel()->refresh();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void PurchaseItemWidget::addClicked()
{
    mAddDialog->reset();
    mAddDialog->show();
}

void PurchaseItemWidget::updateClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        mAddDialog->fill(item->data());
        mAddDialog->show();
    }
}

void PurchaseItemWidget::delClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        int res = QMessageBox::question(this, tr("Confirmation remove"), tr("Are you sure to delete the item?"));
        if(res == QMessageBox::Yes) {
            Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::DEL);
            msg.addData("id", item->id);
            sendMessage(&msg);
        }
    }
}
