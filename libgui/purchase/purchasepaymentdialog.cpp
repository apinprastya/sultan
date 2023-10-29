/*
 * purchasepaymentdialog.cpp
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
#include "purchasepaymentdialog.h"
#include "db_constant.h"
#include "dbutil.h"
#include "global_constant.h"
#include "guiutil.h"
#include "message.h"
#include "preference.h"
#include "purchaseaddpaymentdialog.h"
#include "rowdata.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "ui_purchasepaymentdialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchasePaymentDialog::PurchasePaymentDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::PurchasePaymentDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->tableWidget->initCrudButton();
    ui->tableWidget->getTableView()->setUseStandardHeader(true);
    auto model = ui->tableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("date", tr("Date"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy");
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("bank", tr("Bank"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        const QString &str = item->data(key).toString();
        if (str.isEmpty())
            return tr("Cash");
        else
            return str;
    });
    model->addColumnMoney("money_total", tr("Total"), true);

    model->setFilter("link_type", COMPARE::EQUAL, TRANSACTION_LINK_TYPE::PURCHASE);
    model->setTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::TRANSACTION, MSG_COMMAND::GET);
    ui->tableWidget->setupTable();
    GuiUtil::setColumnWidth(ui->tableWidget->getTableView(), QList<int>() << 100 << 150 << 150 << 100);
    ui->tableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);

    connect(ui->tableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(ui->tableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
    connect(ui->tableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(calculateReceived()));

    model->setSort("created_at ASC");
}

PurchasePaymentDialog::~PurchasePaymentDialog() { delete ui; }

void PurchasePaymentDialog::fill(const QVariantMap &data) {
    ui->labelSuplier->setText(data["suplier"].toString());
    ui->labelTotal->setText(Preference::formatMoney(data["final"].toDouble()));
    mTotal = data["final"].toDouble();
    mId = data["id"].toInt();
    mData = data;

    auto model = ui->tableWidget->getModel();
    model->setFilter("link_id", COMPARE::EQUAL, mId);
    model->refresh();
}

void PurchasePaymentDialog::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::DELETE_PAYMENT)) {
        if (msg->isSuccess()) {
            auto model = ui->tableWidget->getModel();
            model->refresh();
        }
    }
}

void PurchasePaymentDialog::addClicked() {
    auto model = ui->tableWidget->getModel();
    auto rowData = model->getRowData();
    double paymentTotal = 0;
    for (int i = 0; i < rowData->size(); i++) {
        paymentTotal += rowData->at(i)->data("money_total").toDouble();
    }
    PurchaseAddPaymentDialog addPaymentDialog(mMessageBus, mId, mTotal + paymentTotal, mData, QVariantMap(), this);
    addPaymentDialog.exec();
    model->refresh();
}

void PurchasePaymentDialog::calculateReceived() {
    auto model = ui->tableWidget->getModel();
    auto rowData = model->getRowData();
    double paymentTotal = 0;
    for (int i = 0; i < rowData->size(); i++) {
        paymentTotal += rowData->at(i)->data("money_total").toDouble();
    }
    ui->labelReceived->setText(Preference::formatMoney(-paymentTotal));
    ui->labelResidual->setText(Preference::formatMoney(mTotal + paymentTotal));
}

void PurchasePaymentDialog::updateClicked(const QModelIndex &index) {
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        PurchaseAddPaymentDialog addPaymentDialog(mMessageBus, mId, 0, mData, item->data(), this);
        addPaymentDialog.exec();
        auto model = ui->tableWidget->getModel();
        model->refresh();
    }
}

void PurchasePaymentDialog::deleteClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    int ret = QMessageBox::question(this, tr("Delete Confirmation"), tr("Are you sure to delete the payment?"));
    if (ret == QMessageBox::Yes) {
        auto item = static_cast<TableItem *>(index.first().internalPointer());
        Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::DELETE_PAYMENT);
        msg.addData("id", item->id);
        msg.addData("purchaseId", mId);
        sendMessage(&msg);
    }
}
