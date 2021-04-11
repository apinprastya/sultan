/*
 * customerwidget.cpp
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
#include "customerwidget.h"
#include "customeradddialog.h"
#include "db_constant.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "horizontalheader.h"
#include "message.h"
#include "preference.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "tilewidget.h"
#include "ui_normalwidget.h"
#include "usersession.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

using namespace LibGUI;
using namespace LibG;

CustomerWidget::CustomerWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTableWidget(new TableWidget(this)),
      mAddDialog(new CustomerAddDialog(bus, this)), mTileCredit(new TileWidget(this)) {
    setMessageBus(bus);
    ui->setupUi(this);
    auto hbox = new QHBoxLayout;
    mTileCredit->setTitleValue(tr("Total Credit"), "loading...");
    hbox->addWidget(mTileCredit);
    hbox->addStretch();
    ui->verticalLayout->addLayout(hbox);
    ui->labelTitle->setText(tr("Customer"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("number", tr("Number"));
    model->addColumn("name", tr("Name"));
    model->addColumn("phone", tr("Phone"));
    model->addColumn("email", tr("Email"));
    model->addColumn("address", tr("Address"));
    model->addColumnMoney("reward", tr("Reward"));
    model->addColumnMoney("credit", tr("Credit"));
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("phone", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("email", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    auto button = new QPushButton(QIcon(":/images/16x16/money-arrow.png"), "");
    button->setToolTip(tr("Credit"));
    button->setFlat(true);
    mTableWidget->addActionButton(button);
    connect(button, SIGNAL(clicked(bool)), SLOT(creditClicked()));
    button = new QPushButton(QIcon(":/images/16x16/rosette-label.png"), "");
    button->setToolTip(tr("Reward"));
    button->setFlat(true);
    mTableWidget->addActionButton(button);
    connect(button, SIGNAL(clicked(bool)), SLOT(rewardClicked()));
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
    connect(mAddDialog, SIGNAL(customerAdded()), SLOT(customerAdded()));
    connect(mAddDialog, SIGNAL(customerUpdated(int)), SLOT(customerUpdated(int)));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
}

void CustomerWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::DEL)) {
        if (msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Customer deleted successfully"));
            mTableWidget->getModel()->refresh();
        } else {
            FlashMessageManager::showError(msg->data("error").toString());
        }
    } else if (msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::SUMMARY)) {
        mTileCredit->setValue(Preference::formatMoney(msg->data("credit").toDouble()));
    }
}

void CustomerWidget::addClicked() {
    mAddDialog->reset();
    mAddDialog->show();
}

void CustomerWidget::updateClicked(const QModelIndex &index) {
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    mAddDialog->fill(item->data());
    mAddDialog->show();
}

void CustomerWidget::deleteClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    int ret =
        QMessageBox::question(this, tr("Delete Confirmation"),
                              tr("Make sure the credit is 0 before delete. Are you sure to delete the customer?"));
    if (ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for (int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem *>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}

void CustomerWidget::customerAdded() { mTableWidget->getModel()->refresh(); }

void CustomerWidget::customerUpdated(int id) { mTableWidget->getModel()->resfreshOne(id); }

void CustomerWidget::creditClicked() {
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    emit requestOpenCustomerCredit(item->id.toInt(), item->data("number").toString());
}

void CustomerWidget::rewardClicked() {
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    emit requestOpenCustomerReward(item->id.toInt(), item->data("number").toString());
}

void CustomerWidget::refreshSummary() {
    Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::SUMMARY);
    sendMessage(&msg);
}
