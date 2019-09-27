/*
 * machinewidget.cpp
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
#include "machinewidget.h"
#include "ui_normalwidget.h"
#include "horizontalheader.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableitem.h"
#include "global_constant.h"
#include "message.h"
#include "tableview.h"
#include "guiutil.h"
#include "flashmessagemanager.h"
#include "addmachinedialog.h"
#include <QMessageBox>
#include <QInputDialog>

using namespace LibGUI;
using namespace LibG;

MachineWidget::MachineWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    setMessageBus(bus);
    ui->setupUi(this);
    ui->labelTitle->setText(tr("Machines"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("code", tr("Code"));
    model->addColumn("name", tr("Name"));
    model->setTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 100 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(editClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
}

void MachineWidget::messageReceived(Message *msg)
{
    if(msg->isSuccess()) {
        if(msg->isTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::INSERT)) {
            FlashMessageManager::showMessage(tr("Machine added successfully"));
        } else if(msg->isTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::UPDATE)) {
            FlashMessageManager::showMessage(tr("Machine updated successfully"));
        } else if(msg->isTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::DEL)) {
            FlashMessageManager::showMessage(tr("Machine deleted successfully"));
        }
        mTableWidget->getModel()->refresh();
    } else {
        QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
    }
}

void MachineWidget::addClicked()
{
    AddMachineDialog dialog(mMessageBus, this);
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void MachineWidget::editClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    AddMachineDialog dialog(mMessageBus, this);
    dialog.fill(item->data());
    dialog.exec();
    mTableWidget->getModel()->refresh();
    /*const QString &name = QInputDialog::getText(this, tr("Edit machine"), tr("Input machine name"), QLineEdit::Normal, item->data("name").toString());
    if(!name.isEmpty() && name.compare(item->data("name").toString())) {
        Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::UPDATE);
        msg.addData("id", item->id);
        msg.addData("data", QVariantMap{{"name", name}});
        sendMessage(&msg);
    }*/
}

void MachineWidget::deleteClicked(const QModelIndexList &index)
{
    if(index.empty()) return;
    int ret = QMessageBox::question(this, tr("Delete Confirmation"), tr("Are you sure to delete?"));
    if(ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for(int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem*>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}
