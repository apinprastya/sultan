/*
 * suplierwidget.cpp
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
#include "suplierwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"
#include "suplieradddialog.h"
#include "tableitem.h"
#include "message.h"
#include "guiutil.h"
#include "tableview.h"
#include "headerwidget.h"
#include "flashmessagemanager.h"
#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

SuplierWidget::SuplierWidget(MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mAddDialog(new SuplierAddDialog(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Suplier"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("name", tr("Name"));
    model->addColumn("code", tr("Code"));
    model->addColumn("phone", tr("Phone"));
    model->addColumn("email", tr("Email"));
    model->addColumn("address", tr("Address"));
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("code", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("phone", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("email", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 150 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(editClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
    connect(mAddDialog, SIGNAL(saveData(QVariantMap,int)), SLOT(saveRequested(QVariantMap,int)));
}

void SuplierWidget::messageReceived(Message *msg)
{
    if(msg->isCommand(MSG_COMMAND::INSERT)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Suplier added successfully"));
            mAddDialog->hide();
            mTableWidget->getModel()->refresh();
        } else {
            QMessageBox::critical(mAddDialog, tr("Error"), msg->data("error").toString());
            mAddDialog->enableSave();
            return;
        }
    } else if(msg->isCommand(MSG_COMMAND::UPDATE)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Suplier updated successfully"));
            mAddDialog->hide();
            mTableWidget->getModel()->refresh();
        } else {
            QMessageBox::critical(mAddDialog, tr("Error"), msg->data("error").toString());
            mAddDialog->enableSave();
            return;
        }
    } else if(msg->isCommand(MSG_COMMAND::DEL) && msg->isSuccess()) {
        FlashMessageManager::showMessage(tr("Suplier deleted successfully"));
        mTableWidget->getModel()->refresh();
    }
}

void SuplierWidget::addClicked()
{
    mAddDialog->reset();
    mAddDialog->show();
}

void SuplierWidget::editClicked(const QModelIndex &index)
{
    auto item = static_cast<TableItem*>(index.internalPointer());
    mAddDialog->fill(item->data());
    mAddDialog->show();
}

void SuplierWidget::deleteClicked(const QModelIndexList &index)
{
    if(index.empty()) return;
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure want to remove suplier?"));
    if(ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for(int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem*>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}

void SuplierWidget::saveRequested(const QVariantMap &data, int id)
{
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::INSERT);
    if(id <= 0) {
        msg.setData(data);
    } else {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", id);
        msg.addData("data", data);
    }
    sendMessage(&msg);
}
