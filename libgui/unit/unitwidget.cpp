/*
 * unitwidget.cpp
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
#include "unitwidget.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "message.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "ui_normalwidget.h"
#include "util.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

UnitWidget::UnitWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTableWidget(new TableWidget(this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Unit"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("name", tr("Name"));
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    ;
    model->setTypeCommand(MSG_TYPE::UNIT, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(editClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
    // connect(mAddDialog, SIGNAL(saveData(QVariantMap,int)), SLOT(saveRequested(QVariantMap,int)));
}

void UnitWidget::messageReceived(LibG::Message *msg) {
    if (msg->isSuccess()) {
        if (msg->isType(MSG_TYPE::UNIT)) {
            if (msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Unit added successfully"));
            } else if (msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Unit updated successfully"));
            } else {
                FlashMessageManager::showMessage(tr("Unit deleted successfully"));
            }
            mTableWidget->getModel()->refresh();
        }
    } else {
        FlashMessageManager::showError(msg->data("error").toString());
    }
}

void UnitWidget::addClicked() {
    const QString &name = QInputDialog::getText(this, tr("Add new unit"), tr("Input name"));
    if (!name.isEmpty()) {
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::INSERT);
        msg.addData("name", Util::capitalize(name));
        sendMessage(&msg);
    }
}

void UnitWidget::editClicked(const QModelIndex &index) {
    auto item = static_cast<TableItem *>(index.internalPointer());
    const QString &name = QInputDialog::getText(this, tr("Add new unit"), tr("Input name"), QLineEdit::Normal,
                                                item->data("name").toString());
    if (!name.isEmpty()) {
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::UPDATE);
        msg.addData("id", item->id);
        msg.addData("data", QVariantMap{{"name", Util::capitalize(name)}});
        sendMessage(&msg);
    }
}

void UnitWidget::deleteClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure want to remove unit?"));
    if (ret == QMessageBox::Yes) {
        QList<QVariant> ids;
        for (int i = 0; i < index.size(); i++) {
            auto item = static_cast<TableItem *>(index[i].internalPointer());
            ids.append(item->id);
        }
        Message msg(MSG_TYPE::UNIT, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}
