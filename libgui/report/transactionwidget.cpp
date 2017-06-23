/*
 * transactionwidget.cpp
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
#include "transactionwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "global_constant.h"
#include "guiutil.h"
#include "db_constant.h"
#include "message.h"
#include "headerwidget.h"
#include "tilewidget.h"
#include "message.h"
#include "preference.h"
#include "dbutil.h"
#include "util.h"
#include <QHBoxLayout>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

TransactionWidget::TransactionWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mTileIncome(new TileWidget(this)),
    mTileExpense(new TileWidget(this)),
    mTileDifference(new TileWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Transactions"));

    auto hor = new QHBoxLayout;
    mTileIncome->setTitleValue(tr("Income"), "loading...");
    hor->addWidget(mTileIncome);
    mTileExpense->setTitleValue(tr("Expense"), "loading...");
    hor->addWidget(mTileExpense);
    mTileDifference->setTitleValue(tr("Income - Expense"), "loading...");
    hor->addWidget(mTileDifference);
    hor->addStretch();
    ui->verticalLayout->addLayout(hor);
    ui->verticalLayout->addWidget(mTableWidget);

    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh);
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("date", tr("Date"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    });
    model->addColumn("type", tr("Type"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return item->data(key).toInt() == TRANSACTION_TYPE::INCOME ? tr("Income") : tr("Expense");
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("detail", tr("Detail"));
    model->addColumnMoney("transaction_total", tr("Total"));
    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("date", HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("detail", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("type", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->setTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::QUERY);
    model->setFilter("transaction_total", COMPARE::NEQUAL, 0);
    model->setSort("date DESC");
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 100 << 200 << 300 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
}

void TransactionWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_TRANSACTION)) {
        if(msg->isSuccess()) {
            mTileIncome->setValue(Preference::toString(msg->data("income").toDouble()));
            mTileExpense->setValue(Preference::toString(msg->data("expense").toDouble()));
            mTileDifference->setValue(Preference::toString(msg->data("total").toDouble()));
        }
    }
}

void TransactionWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if(isShowed) return;
    isShowed = true;
    auto combo = mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("type"))->getComboBox();
    combo->clear();
    combo->addItem(tr("All"), -1);
    combo->addItem(tr("Income"), TRANSACTION_TYPE::INCOME);
    combo->addItem(tr("Expense"), TRANSACTION_TYPE::EXPENSE);
}

void TransactionWidget::refreshSummary()
{
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_TRANSACTION);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}
