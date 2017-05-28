/*
 * saleswidget.cpp
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
#include "saleswidget.h"
#include "ui_saleswidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "headerwidget.h"
#include "global_constant.h"
#include "guiutil.h"
#include "dbutil.h"
#include "util.h"
#include "tableitem.h"
#include "message.h"
#include "querydb.h"
#include "preference.h"
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

SalesWidget::SalesWidget(LibG::MessageBus *bus, QWidget *parent):
    QWidget(parent),
    ui(new Ui::SalesWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Sales Report"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh);
    auto dateFormater = [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    };
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date"), Qt::AlignLeft, dateFormater);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumnMoney("count", tr("Count"));
    model->addColumnMoney("total", tr("Total"));
    model->addColumnMoney("buy_price", tr("Buy Price"));
    model->addColumnMoney("margin", tr("Margin"));
    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("created_at", HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 200 << 70 << 100 << 100 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
}

void SalesWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_SUMMARY) && msg->isSuccess()) {
        ui->labelSales->setText(Preference::toString(msg->data("total").toDouble()));
        ui->labelMargin->setText(Preference::toString(msg->data("margin").toDouble()));
    }
}

void SalesWidget::refreshSummary()
{
    Message msg(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_SUMMARY);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}
