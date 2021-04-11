/*
 * bankwidget.cpp
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
#include "bankwidget.h"
#include "bank/bankadddialog.h"
#include "db_constant.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "horizontalheader.h"
#include "message.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_normalwidget.h"
#include "usersession.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

BankWidget::BankWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTableWidget(new TableWidget(this)) {
    setMessageBus(bus);
    ui->setupUi(this);
    ui->labelTitle->setText(tr("Banks"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("name", tr("Name"));
    model->addColumn("debit_charge_formula", tr("Debit Charge Formula"));
    model->addColumn("credit_charge_formula", tr("Credit Charge Formula"));
    model->setTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 200 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
}

void BankWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::DEL)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Bank deleted successfully"));
            mTableWidget->getModel()->refresh();
        }
    }
}

void BankWidget::addClicked() {
    BankAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void BankWidget::updateClicked(const QModelIndex &index) {
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    BankAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.fill(item->data());
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void BankWidget::deleteClicked(const QModelIndexList &index) {
    int ret = QMessageBox::question(this, tr("Delete Confirmation"), tr("Are you sure to delete selected the bank?"));
    if (ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for (int i = 0; i < index.length(); i++) {
            auto item = static_cast<TableItem *>(index[i].internalPointer());
            ids.push_back(item->id);
        }
        Message msg(MSG_TYPE::BANK, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}
