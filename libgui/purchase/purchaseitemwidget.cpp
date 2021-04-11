/*
 * purchaseitemwidget.cpp
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
#include "purchaseitemwidget.h"
#include "db_constant.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "message.h"
#include "preference.h"
#include "purchaseadditemdialog.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "tilewidget.h"
#include "ui_normalwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchaseItemWidget::PurchaseItemWidget(const QVariantMap &data, LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTableWidget(new TableWidget(this)),
      mAddDialog(new PurchaseAddItemDialog(bus, data["id"].toInt(), this)), mTileTotal(new TileWidget(this)),
      mTileDiscount(new TileWidget(this)), mTileFinal(new TileWidget(this)) {
    mId = data["id"].toInt();
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(
        tr("Purchase Item : %1 / %2").arg(data["number"].toString()).arg(data["suplier"].toString()));

    auto hor = new QHBoxLayout;
    mTileTotal->setTitleValue(tr("Total"), "0");
    hor->addWidget(mTileTotal);
    mTileDiscount->setTitleValue(tr("Discount"), "0");
    hor->addWidget(mTileDiscount);
    mTileFinal->setTitleValue(tr("Total - Discount"), "0");
    hor->addWidget(mTileFinal);
    hor->addStretch();
    ui->verticalLayout->addLayout(hor);

    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumn("count", tr("Count"), Qt::AlignRight);
    model->addColumnMoney("price", tr("Price"));
    model->addColumnMoney("discount", tr("Discount"));
    model->addColumnMoney("final", tr("Total"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::QUERY);
    model->setTypeCommandOne(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::GET);
    model->setFilter("purchase_id", COMPARE::EQUAL, data["id"].toInt());
    mTableWidget->setupTable();
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>()
                                                              << 150 << 200 << 100 << 100 << 100 << 100 << 100);
    model->refresh();
    ui->verticalLayout->addWidget(mTableWidget);
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(delClicked(QModelIndexList)));
    connect(mAddDialog, SIGNAL(addSuccess()), mTableWidget->getModel(), SLOT(refresh()));
    connect(mAddDialog, SIGNAL(updateSuccess(QVariant)), model, SLOT(resfreshOne(QVariant)));
    connect(mAddDialog, SIGNAL(updateSuccess(QVariant)), SLOT(refreshSummary()));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
}

PurchaseItemWidget::~PurchaseItemWidget() { delete ui; }

void PurchaseItemWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::DEL)) {
        if (msg->isSuccess()) {
            mTableWidget->getModel()->refresh();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    } else if (msg->isTypeCommand(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::SUMMARY)) {
        mTileTotal->setValue(Preference::formatMoney(msg->data("total").toDouble()));
        mTileDiscount->setValue(Preference::formatMoney(msg->data("discount").toDouble()));
        mTileFinal->setValue(Preference::formatMoney(msg->data("final").toDouble()));
    }
}

void PurchaseItemWidget::addClicked() {
    mAddDialog->reset();
    mAddDialog->show();
}

void PurchaseItemWidget::updateClicked(const QModelIndex &index) {
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        mAddDialog->fill(item->data());
        mAddDialog->show();
    }
}

void PurchaseItemWidget::delClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    int res = QMessageBox::question(this, tr("Confirmation remove"), tr("Are you sure to delete the item?"));
    if (res == QMessageBox::Yes) {
        QList<QVariant> ids;
        for (int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem *>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}

void PurchaseItemWidget::refreshSummary() {
    mTileDiscount->setValue(tr("loading..."));
    mTileFinal->setValue(tr("loading..."));
    mTileTotal->setValue(tr("loading..."));
    Message msg(MSG_TYPE::PURCHASE_ITEM, MSG_COMMAND::SUMMARY);
    msg.addData("purchase_id", mId);
    sendMessage(&msg);
}
