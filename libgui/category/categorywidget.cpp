/*
 * categorywidget.cpp
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
#include "categorywidget.h"
#include "ui_normalwidget.h"
#include "categorytreewidget.h"
#include "message.h"
#include "global_constant.h"
#include <QPushButton>
#include <QTimer>

using namespace LibGUI;
using namespace LibG;

CategoryWidget::CategoryWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTreeWidget(new CategoryTreeWidget(this))
{
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
}

CategoryWidget::~CategoryWidget()
{
}

void CategoryWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY)) {
        if(msg->isSuccess()) {
            mTreeWidget->load(msg->data("data").toList());;
        } else {
        }
    }
}

QPushButton *CategoryWidget::addButtonAction(QHBoxLayout *layout, const QString &iconName)
{
    auto button = new QPushButton(this);
    button->setFlat(true);
    button->setIcon(QIcon(iconName));
    layout->addWidget(button);
    return button;
}

void CategoryWidget::loadCategory()
{
    Message msg(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
}

void CategoryWidget::addClicked()
{
}

void CategoryWidget::updateClicked()
{
}

void CategoryWidget::deleteClicked()
{
}
