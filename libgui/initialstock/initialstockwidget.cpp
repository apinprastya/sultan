/*
 *
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

#include "initialstockwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"
#include "guiutil.h"
#include "tableview.h"
#include "tableitem.h"
#include "dbutil.h"
#include "headerwidget.h"
#include "db_constant.h"
#include "message.h"
#include "flashmessagemanager.h"
#include "preference.h"
#include "util.h"
#include "initialstockadddialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

InitialStockWidget::InitialStockWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mAddDialog(new InitialStockAddDialog(bus, this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Initial Stock"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh << TableWidget::Add);
    auto dateFormater = [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    };
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date"), Qt::AlignLeft, dateFormater);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("real_stock", tr("Stock"), Qt::AlignRight);
    //model->addColumn("note", tr("Note"));
    model->setFilter("flag", COMPARE::FLAG_ENABLE, CHECKSTOCK_FLAG::INITIAL);

    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("created_at", HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});

    model->setTypeCommand(MSG_TYPE::CHECKSTOCK, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::CHECKSTOCK, MSG_COMMAND::GET);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 150 << 100 << 100 << 75 << 75 << 75 << 200);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->setSort("created_at DESC");
    model->refresh();

    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mAddDialog, SIGNAL(addSuccess()), model, SLOT(refresh()));
}

void InitialStockWidget::messageReceived(LibG::Message */*msg*/)
{

}

void InitialStockWidget::addClicked()
{
    mAddDialog->reset();
    mAddDialog->show();
}

