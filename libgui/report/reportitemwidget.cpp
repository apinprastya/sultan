/*
 * reportitemwidget.cpp
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
#include "reportitemwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "headerwidget.h"
#include "global_constant.h"
#include "db_constant.h"
#include "guiutil.h"
#include <QDate>

using namespace LibGUI;
using namespace LibG;

ReportItemWidget::ReportItemWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh);
    ui->labelTitle->setText(tr("Item sales"));
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("stock", tr("Stock"));
    model->addColumn("count", tr("Sold"));
    model->addColumn("category", tr("Category"));
    model->addColumn("suplier", tr("Suplier"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_ITEM_REPORT);
    model->setTypeCommandOne(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::GET);
    QDate prev = QDate::currentDate().addDays(-7);
    model->setFilter("0$solditems.created_at", COMPARE::GREATER_EQUAL, prev);
    model->setFilter("1$solditems.created_at", COMPARE::LESS_EQUAL, QDate::currentDate());
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 100 << 100 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
}

ReportItemWidget::~ReportItemWidget()
{

}

void ReportItemWidget::messageReceived(LibG::Message */*msg*/)
{

}
