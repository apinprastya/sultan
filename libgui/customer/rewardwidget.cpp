/*
 * rewardwidget.cpp
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
#include "rewardwidget.h"
#include "ui_normalwidget.h"
#include "horizontalheader.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableitem.h"
#include "global_constant.h"
#include "message.h"
#include "tableview.h"
#include "usersession.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "db_constant.h"
#include "flashmessagemanager.h"
#include "rewardadddialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

RewardWidget::RewardWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    setMessageBus(bus);
    ui->setupUi(this);
    ui->labelTitle->setText(tr("Rewards"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    mTableWidget->getTableView()->setUseStandardHeader(true);
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("count", tr("Poin"));
    model->addColumn("detail", tr("Detail"));
    model->setTypeCommand(MSG_TYPE::REWARD, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->setSort("count ASC");
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndex)), SLOT(deleteClicked(QModelIndex)));
}

void RewardWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::REWARD, MSG_COMMAND::DEL)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Reward deleted successfully"));
            mTableWidget->getModel()->refresh();
        } else {
            FlashMessageManager::showError(tr("Error delete : %1").arg(msg->data("error").toString()));
        }
    }
}

void RewardWidget::addClicked()
{
    RewardAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void RewardWidget::updateClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    RewardAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.fill(item->data());
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void RewardWidget::deleteClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure to delete?"));
    if(ret == QMessageBox::Yes) {
        Message msg(MSG_TYPE::REWARD, MSG_COMMAND::DEL);
        msg.addData("id", item->id);
        sendMessage(&msg);
    }
}
