/*
 * searchitemdialog.cpp
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
#include "searchitemdialog.h"
#include "db_constant.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "keyevent.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_searchitemdialog.h"
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

SearchItemDialog::SearchItemDialog(MessageBus *bus, bool advance, QWidget *parent)
    : QDialog(parent), ui(new Ui::SearchItemDialog), mTableWidget(new TableWidget(this, !advance)),
      mIsAdvance(advance) {
    ui->setupUi(this);
    auto keyEvent = new KeyEvent(this);
    keyEvent->addConsumeKey(Qt::Key_Return);
    mTableWidget->installEventFilter(keyEvent);
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumnMoney("sell_price", tr("Price"));
    model->addColumnMoney("stock", tr("Stock"));
    model->setTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::QUERY);
    mTableWidget->setDefaultPerPage(3, false);
    mTableWidget->setupTable();
    ui->verticalLayout->addWidget(mTableWidget);
    connect(ui->lineName, SIGNAL(returnPressed()), SLOT(nameDone()));
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 200 << 200 << 100 << 80);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->lineName->setFocus();
    connect(mTableWidget->getModel(), SIGNAL(firstDataLoaded()), SLOT(dataLoaded()));
    connect(keyEvent, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(returnPressed()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(doubleClicked(QModelIndex)));
    if (advance) {
        ui->lineName->hide();
        model->addHeaderFilter("barcode",
                               HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLikeNative, QVariant()});
        model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
        model->refresh();
    }
}

SearchItemDialog::~SearchItemDialog() { delete ui; }

void SearchItemDialog::setNameField(const QString &str) {
    if (str.contains("*")) {
        auto l = str.split("*");
        if (l.size() > 1)
            ui->lineName->setText(l[1]);
    } else {
        ui->lineName->setText(str);
    }
    nameDone();
}

void SearchItemDialog::nameDone() {
    const QString &name = ui->lineName->text();
    if (name.isEmpty())
        return;
    mTableWidget->getModel()->setFilter("name", COMPARE::LIKE, name);
    mTableWidget->getModel()->refresh();
}

void SearchItemDialog::dataLoaded() {
    if (mTableWidget->getModel()->rowCount(QModelIndex()) > 0) {
        mTableWidget->getTableView()->selectRow(0);
        mTableWidget->getTableView()->setFocus(Qt::TabFocusReason);
    }
}

void SearchItemDialog::returnPressed() {
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        mSelectedBarcode = item->data("barcode").toString();
        mSelectedData = item->data();
        close();
    }
}

void SearchItemDialog::doubleClicked(const QModelIndex &index) {
    auto item = static_cast<TableItem *>(index.internalPointer());
    if (item) {
        if (mIsAdvance) {
            mSelectedData = item->data();
            mSelectedBarcode = item->data("barcode").toString();
            mIsOk = true;
        } else {
            mSelectedBarcode = item->data("barcode").toString();
            mSelectedData = item->data();
        }
        close();
    }
}
