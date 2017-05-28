/*
 * purchasewidget.cpp
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
#include "purchasewidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"
#include "guiutil.h"
#include "tableview.h"
#include "tableitem.h"
#include "purchaseadddialog.h"
#include "dbutil.h"
#include "headerwidget.h"
#include "db_constant.h"

using namespace LibGUI;
using namespace LibG;

PurchaseWidget::PurchaseWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mAddDialog(new PurchaseAddDialog(bus, this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Purchase"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    auto dateFormater = [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDate(item->data(key).toString()).toString("dd-MM-yyyy");
    };
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date"), Qt::AlignLeft, dateFormater);
    model->addColumn("number", tr("Number"));
    model->addColumn("suplier", tr("Suplier"));
    model->addColumn("deadline", tr("Deadline"), Qt::AlignLeft, dateFormater);
    model->addColumnMoney("total", tr("Total"));
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("suplier", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::PURCHASE, MSG_COMMAND::GET);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 200 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndex)), SLOT(deleteClicked(QModelIndex)));
    connect(mAddDialog, SIGNAL(successAdd()), mTableWidget->getModel(), SLOT(refresh()));
    connect(mAddDialog, SIGNAL(successUpdate(QVariant)), mTableWidget->getModel(), SLOT(resfreshOne(QVariant)));
    connect(mTableWidget->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(tableDoubleClicked(QModelIndex)));
}

PurchaseWidget::~PurchaseWidget()
{

}

void PurchaseWidget::messageReceived(LibG::Message */*msg*/)
{

}

void PurchaseWidget::addClicked()
{
    mAddDialog->reset();
    mAddDialog->show();
}

void PurchaseWidget::updateClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    mAddDialog->fill(item->data());
    mAddDialog->show();
}

void PurchaseWidget::deleteClicked(const QModelIndex &index)
{

}

void PurchaseWidget::tableDoubleClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        emit requestOpenPurchaseWidget(item->id.toInt(), item->data("number").toString());
    }
}
