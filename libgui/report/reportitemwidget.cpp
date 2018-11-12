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
#include "ui_datefromtowidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "headerwidget.h"
#include "global_constant.h"
#include "db_constant.h"
#include "guiutil.h"
#include <QDateTime>

using namespace LibGUI;
using namespace LibG;

ReportItemWidget::ReportItemWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mDateWidget(new Ui::DateStartEndWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    auto widget = new QWidget(this);
    mDateWidget->setupUi(widget);
    ui->horizontalLayout->addWidget(widget);
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh);
    ui->labelTitle->setText(tr("Item sales"));
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("stock", tr("Stock"), Qt::AlignRight);
    model->addColumn("count", tr("Sold"), Qt::AlignRight);
    model->addColumn("unit", tr("Unit"));
    model->addColumn("category", tr("Category"));
    model->addColumn("suplier", tr("Suplier"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_ITEM_REPORT);
    model->setTypeCommandOne(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::GET);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 100 << 100 << 100 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    auto now = QDateTime::currentDateTime();
    now.setTime(QTime(23, 59, 59));
    mDateWidget->dateEnd->setDateTime(now);
    mDateWidget->dateStart->setDateTime(now.addDays(-7));
    connect(mDateWidget->dateStart, SIGNAL(dateTimeChanged(QDateTime)), SLOT(refreshTable()));
    connect(mDateWidget->dateEnd, SIGNAL(dateTimeChanged(QDateTime)), SLOT(refreshTable()));
    refreshTable();
}

ReportItemWidget::~ReportItemWidget()
{

}

void ReportItemWidget::messageReceived(LibG::Message */*msg*/)
{

}

void ReportItemWidget::refreshTable()
{
    auto model = mTableWidget->getModel();
    model->setFilter("0$solditems.created_at", COMPARE::GREATER_EQUAL, mDateWidget->dateStart->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    model->setFilter("1$solditems.created_at", COMPARE::LESS_EQUAL, mDateWidget->dateEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    model->refresh();
}
