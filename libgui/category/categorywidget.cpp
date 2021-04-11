/*
 * categorywidget.cpp
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
#include "categorywidget.h"
#include "categorytreewidget.h"
#include "categotyadddialog.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "message.h"
#include "ui_normalwidget.h"
#include <QComboBox>
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItem>

using namespace LibGUI;
using namespace LibG;

CategoryWidget::CategoryWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTreeWidget(new CategoryTreeWidget(this)),
      mAddDialog(new CategoryAddDialog(this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Category"));
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *btn = addButtonAction(buttonLayout, ":/images/16x16/plus.png");
    connect(btn, SIGNAL(clicked(bool)), SLOT(addClicked()));
    btn = addButtonAction(buttonLayout, ":/images/16x16/pencil.png");
    connect(btn, SIGNAL(clicked(bool)), SLOT(updateClicked()));
    btn = addButtonAction(buttonLayout, ":/images/16x16/cross.png");
    connect(btn, SIGNAL(clicked(bool)), SLOT(deleteClicked()));
    btn = addButtonAction(buttonLayout, ":/images/16x16/refresh.png");
    connect(btn, SIGNAL(clicked(bool)), SLOT(loadCategory()));
    ui->verticalLayout->addWidget(mTreeWidget);
    ui->verticalLayout->addLayout(buttonLayout);
    QTimer::singleShot(10, this, SLOT(loadCategory()));
    connect(mAddDialog, SIGNAL(saveRequest(QVariantMap, int)), SLOT(saveRequested(QVariantMap, int)));
}

CategoryWidget::~CategoryWidget() {}

void CategoryWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY)) {
        if (msg->isSuccess()) {
            mTreeWidget->load(msg->data("data").toList());
            ;
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    } else if (msg->isType(MSG_TYPE::CATEGORY)) {
        if (msg->isSuccess()) {
            mAddDialog->hide();
            if (msg->isCommand(MSG_COMMAND::INSERT)) {
                FlashMessageManager::showMessage(tr("Category added successfully"));
                auto item = mTreeWidget->addItem(msg->data());
                mTreeWidget->setCurrentItem(item);
            } else if (msg->isCommand(MSG_COMMAND::UPDATE)) {
                FlashMessageManager::showMessage(tr("Category updated successfully"));
                mTreeWidget->updateItem(msg->data());
            } else if (msg->isCommand(MSG_COMMAND::DEL)) {
                FlashMessageManager::showMessage(tr("Category deleted successfully"));
                mTreeWidget->deleteItem(mDeletedId);
            }
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            mAddDialog->enableSaveButton(true);
        }
    }
}

QPushButton *CategoryWidget::addButtonAction(QHBoxLayout *layout, const QString &iconName) {
    auto button = new QPushButton(this);
    button->setFlat(true);
    button->setIcon(QIcon(iconName));
    layout->addWidget(button);
    return button;
}

void CategoryWidget::pupulateComboBox() {
    auto combo = mAddDialog->getComboParent();
    combo->clear();
    combo->addItem("ROOT", 0);
    const QList<CategoryData> &data = mTreeWidget->getData();
    for (const CategoryData &c : data) {
        combo->addItem(c.name, c.id);
    }
}

void CategoryWidget::loadCategory() {
    Message msg(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
}

void CategoryWidget::addClicked() {
    mAddDialog->reset();
    pupulateComboBox();
    mAddDialog->show();
}

void CategoryWidget::updateClicked() {
    auto item = mTreeWidget->currentItem();
    if (item != nullptr) {
        const QString &name = item->data(0, Qt::DisplayRole).toString();
        const QString &code = item->data(1, Qt::DisplayRole).toString();
        int id = item->data(0, Qt::UserRole).toInt();
        int parent = 0;
        auto p = item->parent();
        if (p != nullptr) {
            parent = p->data(0, Qt::UserRole).toInt();
        }
        pupulateComboBox();
        mAddDialog->fill(id, parent, name, code);
        mAddDialog->show();
    }
}

void CategoryWidget::deleteClicked() {
    auto item = mTreeWidget->currentItem();
    if (item != nullptr) {
        int ret = QMessageBox::question(this, tr("Delete confirmation"),
                                        tr("This will also delete the childrens. Sure to delete category?"));
        if (ret == QMessageBox::Yes) {
            mDeletedId = item->data(0, Qt::UserRole).toInt();
            Message msg(MSG_TYPE::CATEGORY, MSG_COMMAND::DEL);
            msg.addData("id", mDeletedId);
            sendMessage(&msg);
        }
    }
}

void CategoryWidget::saveRequested(const QVariantMap &data, int id) {
    Message msg(MSG_TYPE::CATEGORY, MSG_COMMAND::INSERT);
    if (id > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", id);
        msg.addData("data", data);
    } else {
        msg.setData(data);
    }
    sendMessage(&msg);
}
