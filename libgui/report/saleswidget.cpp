/*
 * saleswidget.cpp
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
#include "saleswidget.h"
#include "db_constant.h"
#include "dbutil.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "message.h"
#include "preference.h"
#include "querydb.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_saleswidget.h"
#include "util.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

using namespace LibGUI;
using namespace LibG;

SalesWidget::SalesWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::SalesWidget), mTableWidget(new TableWidget(this)) {
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
    model->addColumn("count", tr("Count"), Qt::AlignRight);
    model->addColumn("unit", tr("Unit"));
    model->addColumnMoney("final", tr("Total"));
    model->addColumnMoney("buy_price", tr("Buy Price"));
    model->addColumnMoney("margin", tr("Margin"));
    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("created_at",
                           HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>()
                                                              << 150 << 150 << 200 << 60 << 75 << 100 << 100 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    model->setFilter("flag", COMPARE::FLAG_DISABLE, ITEM_FLAG::ITEM_LINK);
    // model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
    auto button = new QPushButton(QIcon(":/images/16x16/drive-download.png"), "");
    button->setFlat(true);
    button->setToolTip(tr("Export"));
    connect(button, SIGNAL(clicked(bool)), SLOT(exportClicked()));
    mTableWidget->addActionButton(button);
}

void SalesWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_SUMMARY) && msg->isSuccess()) {
        ui->labelSales->setText(Preference::formatMoney(msg->data("total").toDouble()));
        ui->labelMargin->setText(Preference::formatMoney(msg->data("margin").toDouble()));
    } else if (msg->isTypeCommand(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::EXPORT) && msg->isSuccess()) {
        const QString &fileName = QFileDialog::getSaveFileName(this, tr("Save as"), QDir::homePath(), "*.csv");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QFile::WriteOnly)) {
                file.write(msg->data("data").toString().toUtf8());
                file.close();
            } else {
                QMessageBox::critical(this, tr("Error"), tr("Unable to save to file"));
            }
        }
    }
}

void SalesWidget::refreshSummary() {
    Message msg(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::SOLD_SUMMARY);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}

void SalesWidget::exportClicked() {
    Message msg(MSG_TYPE::SOLD_ITEM, MSG_COMMAND::EXPORT);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}
