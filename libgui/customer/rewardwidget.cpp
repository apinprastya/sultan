/*
 * rewardwidget.cpp
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
#include "rewardwidget.h"
#include "ui_rewardsetting.h"
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
#include "settingpoinadddialog.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

RewardWidget::RewardWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RewardSetting)
{
    setMessageBus(bus);
    ui->setupUi(this);
    ui->tableExchange->initCrudButton();
    ui->tableExchange->getTableView()->setUseStandardHeader(true);
    auto model = ui->tableExchange->getModel();
    model->setMessageBus(bus);
    model->addColumn("count", tr("Poin"), Qt::AlignHCenter);
    model->addColumn("detail", tr("Detail"));
    model->setTypeCommand(MSG_TYPE::REWARD, MSG_COMMAND::QUERY);
    ui->tableExchange->setupTable();
    GuiUtil::setColumnWidth(ui->tableExchange->getTableView(), QList<int>() << 50 << 150);
    ui->tableExchange->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->setSort("count ASC");
    model->refresh();
    connect(ui->tableExchange, SIGNAL(addClicked()), SLOT(addExchangeClicked()));
    connect(ui->tableExchange, SIGNAL(updateClicked(QModelIndex)), SLOT(updateExchangeClicked(QModelIndex)));
    connect(ui->tableExchange, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteExchangeClicked(QModelIndexList)));

    ui->tablePoin->initCrudButton();
    ui->tablePoin->getTableView()->setUseStandardHeader(true);
    model = ui->tablePoin->getModel();
    model->setMessageBus(bus);
    model->addColumnMoney("total", tr("Total"));
    model->addColumnMoney("poin", tr("Reward to get"));
    model->setTypeCommand(MSG_TYPE::REWARD_POIN, MSG_COMMAND::QUERY);
    ui->tablePoin->setupTable();
    GuiUtil::setColumnWidth(ui->tablePoin->getTableView(), QList<int>() << 150 << 150);
    ui->tablePoin->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->setSort("total ASC");
    model->refresh();
    connect(ui->tablePoin, SIGNAL(addClicked()), SLOT(addPoinClicked()));
    connect(ui->tablePoin, SIGNAL(updateClicked(QModelIndex)), SLOT(updatePoinClicked(QModelIndex)));
    connect(ui->tablePoin, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deletePoinClicked(QModelIndexList)));
}

void RewardWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::REWARD, MSG_COMMAND::DEL)) {
        if(msg->isSuccess()) {
            FlashMessageManager::showMessage(tr("Reward deleted successfully"));
            ui->tableExchange->getModel()->refresh();
        } else {
            FlashMessageManager::showError(tr("Error delete : %1").arg(msg->data("error").toString()));
        }
    }
}

void RewardWidget::addExchangeClicked()
{
    RewardAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.exec();
    ui->tableExchange->getModel()->refresh();
}

void RewardWidget::updateExchangeClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    RewardAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.fill(item->data());
    dialog.exec();
    ui->tableExchange->getModel()->refresh();
}

void RewardWidget::deleteExchangeClicked(const QModelIndexList &index)
{
    if(index.empty()) return;
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure to delete?"));
    if(ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for(int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem*>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::REWARD, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}

void RewardWidget::addPoinClicked()
{
    SettingPoinAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.exec();
    ui->tablePoin->getModel()->refresh();
}

void RewardWidget::updatePoinClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    SettingPoinAddDialog dialog(mMessageBus, this);
    dialog.reset();
    dialog.fill(item->data());
    dialog.exec();
    ui->tablePoin->getModel()->refresh();
}

void RewardWidget::deletePoinClicked(const QModelIndexList &index)
{
    if(index.empty()) return;
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure to delete?"));
    if(ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for(int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem*>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::REWARD_POIN, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}
