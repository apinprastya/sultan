/*
 * purchasewidget.cpp
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
#include "purchasewidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"
#include "guiutil.h"
#include "tableview.h"
#include "tableitem.h"
#include "purchaseadddialog.h"
#include "dbutil.h"
#include "headerwidget.h"
#include "db_constant.h"
#include "message.h"
#include "flashmessagemanager.h"
#include "purchasepaymentdialog.h"
#include "tilewidget.h"
#include "preference.h"
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

PurchaseWidget::PurchaseWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mAddDialog(new PurchaseAddDialog(bus, this)),
    mTotalDebit(new TileWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Purchase"));
    mTotalDebit->setTitleValue(tr("Total Debit"), tr("loading..."));
    ui->verticalLayout->addWidget(mTotalDebit);
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    mTableWidget->setEnableDoubleClickUpdate(false);
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy");
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("suplier", tr("Suplier"));
    model->addColumn("payment_type", tr("Type"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return item->data(key).toInt() == PURCHASEPAYMENT::DIRECT ? tr("Direct") : tr("Deadline");
    });
    model->addColumn("deadline", tr("Deadline"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDate(item->data(key).toString()).toString("dd-MM-yyyy");
    });
    model->addColumn("status", tr("Status"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        int type = item->data("payment_type").toInt();
        if(type == PURCHASEPAYMENT::DIRECT) return tr("Paid");
        return (item->data(key) == PAYMENT_STATUS::PAID ? tr("Paid") : tr("Unpaid"));
    });
    model->addColumnMoney("total", tr("Sub-total"));
    model->addColumn("discount_formula", tr("Disc. Form"));
    model->addColumnMoney("discount", tr("Discount"));
    model->addColumnMoney("final", tr("Total"));
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("suplier", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("payment_type", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->addHeaderFilter("status", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->setTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::PURCHASE, MSG_COMMAND::GET);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 200 << 100 << 100 << 100 << 100 << 100 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    auto button = new QPushButton(QIcon(":/images/16x16/money-arrow.png"), "");
    button->setToolTip(tr("Payment"));
    button->setFlat(true);
    connect(button, SIGNAL(clicked(bool)), SLOT(paymentClicked()));
    mTableWidget->addActionButton(button);
    model->setSort("id DESC");
    model->refresh();

    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
    connect(mAddDialog, SIGNAL(successAdd()), mTableWidget->getModel(), SLOT(refresh()));
    connect(mAddDialog, SIGNAL(successUpdate(QVariant)), mTableWidget->getModel(), SLOT(resfreshOne(QVariant)));
    connect(mTableWidget->getTableView(), SIGNAL(doubleClicked(QModelIndex)), SLOT(tableDoubleClicked(QModelIndex)));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(getSummary()));
}

PurchaseWidget::~PurchaseWidget()
{

}

void PurchaseWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if(isShowed) return;
    isShowed = true;
    auto combo = mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("status"))->getComboBox();
    combo->blockSignals(true);
    combo->clear();
    combo->addItem(tr("All"), -1);
    combo->addItem(tr("Unpaid"), PAYMENT_STATUS::UNPAID);
    combo->addItem(tr("Paid"), PAYMENT_STATUS::PAID);
    combo->blockSignals(false);
    combo = mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("payment_type"))->getComboBox();
    combo->blockSignals(true);
    combo->clear();
    combo->addItem(tr("All"), -1);
    combo->addItem(tr("Direct"), PURCHASEPAYMENT::DIRECT);
    combo->addItem(tr("Deadline"), PURCHASEPAYMENT::TEMPO);
    combo->blockSignals(false);
}

void PurchaseWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::DEL)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Purchase deleted successfully"));
            mTableWidget->getModel()->refresh();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    } else if(msg->isTypeCommand(MSG_TYPE::PURCHASE, MSG_COMMAND::SUMMARY)) {
        if(msg->isSuccess()) {
            mTotalDebit->setValue(Preference::formatMoney(msg->data("total").toDouble()));
        }
    }
}

void PurchaseWidget::addClicked()
{
    mAddDialog->reset();
    mAddDialog->show();
}

void PurchaseWidget::updateClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    mAddDialog->fill(item->data());
    mAddDialog->show();
}

void PurchaseWidget::deleteClicked(const QModelIndexList &index)
{
    if(index.empty()) return;
    int res = QMessageBox::question(this, tr("Delete Confirmation"), tr("Are you sure to delete the purchase?"));
    if(res != QMessageBox::Yes) return;
    QList<QVariant> ids;
    for(int i = 0; i < index.size(); i++) {
        auto item = static_cast<TableItem*>(index[i].internalPointer());
        ids.append(item->id);
    }
    Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::DEL);
    msg.addData("id", ids);
    sendMessage(&msg);
}

void PurchaseWidget::tableDoubleClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        auto item = static_cast<TableItem*>(index.internalPointer());
        emit requestOpenPurchaseWidget(item->data());
    }
}

void PurchaseWidget::paymentClicked()
{
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    if(item->data("payment_type").toInt() == PURCHASEPAYMENT::DIRECT) return;
    PurchasePaymentDialog dialog(mMessageBus, this);
    dialog.fill(item->data());
    dialog.exec();
    mTableWidget->getModel()->resfreshOne(item->id);
}

void PurchaseWidget::getSummary()
{
    Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::SUMMARY);
    sendMessage(&msg);
}
