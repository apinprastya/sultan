/*
 * searchcustomerdialog.cpp
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
#include "searchcustomerdialog.h"
#include "db_constant.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "keyevent.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_searchcustomerdialog.h"
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

SearchCustomerDialog::SearchCustomerDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::SearchCustomerDialog) {
    ui->setupUi(this);
    auto keyEvent = new KeyEvent(this);
    keyEvent->addConsumeKey(Qt::Key_Return);
    ui->table->installEventFilter(keyEvent);
    auto model = ui->table->getModel();
    model->setMessageBus(bus);
    model->addColumn("number", tr("ID"));
    model->addColumn("name", tr("Name"));
    model->addColumn("address", tr("Address"));
    model->addColumn("phone", tr("Phone"));
    model->addColumnMoney("reward", tr("Reward"));
    model->addColumnMoney("credit", tr("Credit"));
    model->setTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
    ui->table->setDefaultPerPage(3);
    ui->table->setupTable();
    connect(ui->lineName, SIGNAL(returnPressed()), SLOT(nameDone()));
    GuiUtil::setColumnWidth(ui->table->getTableView(), QList<int>() << 80 << 100 << 100 << 100 << 100 << 100);
    ui->table->getTableView()->horizontalHeader()->setStretchLastSection(true);
    ui->lineName->setFocus();
    ui->table->getTableView()->setUseStandardHeader(true);
    connect(ui->table->getModel(), SIGNAL(firstDataLoaded()), SLOT(dataLoaded()));
    connect(keyEvent, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(returnPressed()));
    connect(ui->table->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(doubleClicked(QModelIndex)));
}

SearchCustomerDialog::~SearchCustomerDialog() { delete ui; }

void SearchCustomerDialog::nameDone() {
    const QString &name = ui->lineName->text();
    if (name.isEmpty())
        return;
    ui->table->getModel()->setFilter("name", COMPARE::LIKE, name);
    ui->table->getModel()->refresh();
}

void SearchCustomerDialog::dataLoaded() {
    if (ui->table->getModel()->rowCount(QModelIndex()) > 0) {
        ui->table->getTableView()->selectRow(0);
        ui->table->getTableView()->setFocus(Qt::TabFocusReason);
    }
}

void SearchCustomerDialog::returnPressed() {
    const QModelIndex &index = ui->table->getTableView()->currentIndex();
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        mSelectedData = item->data();
        mIsOk = true;
        close();
    }
}

void SearchCustomerDialog::doubleClicked(const QModelIndex &index) {
    auto item = static_cast<TableItem *>(index.internalPointer());
    if (item) {
        mSelectedData = item->data();
        mIsOk = true;
        close();
    }
}
