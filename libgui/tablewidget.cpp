/*
 * tablewidget.cpp
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
#include "tablewidget.h"
#include "tableview.h"
#include "tablemodel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

using namespace LibGUI;

static QMap<int, QString> BTNICONS = {
    {TableWidget::Refresh, QStringLiteral(":/images/16x16/refresh.png")},
    {TableWidget::Add, QStringLiteral(":/images/16x16/plus.png")},
    {TableWidget::Update, QStringLiteral(":/images/16x16/pencil.png")},
    {TableWidget::Delete, QStringLiteral(":/images/16x16/cross.png")}
};

TableWidget::TableWidget(QWidget *parent) :
    QWidget(parent),
    mTableView(new TableView(this)),
    mModel(new TableModel(this))
{
    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(mTableView);
    mActionLayout = new QHBoxLayout();
    mActionLayout->addStretch();
    mainLayout->addLayout(mActionLayout);
    setLayout(mainLayout);
    mTableView->setModel(mModel);
}

TableWidget::~TableWidget()
{
}

void TableWidget::initButton(const QList<ButtonType> buttons)
{
    for(int i : buttons) {
        if(BTNICONS.contains(i))
            addActionButton(BTNICONS[i], i);
    }
}

void TableWidget::initCrudButton()
{
    initButton(QList<ButtonType>() << Refresh << Add << Update << Delete);
}

void TableWidget::addActionButton(QPushButton *button)
{
    mActionLayout->addWidget(button);
}

void TableWidget::setupTable()
{
    mModel->reset();
}

void TableWidget::addActionButton(const QString &path, int type)
{
    addActionButton(QIcon(path), type);
}

void TableWidget::addActionButton(const QIcon &icon, int type)
{
    if(mActionButton.contains(type)) return;
    auto push = new QPushButton(this);
    push->setIcon(icon);
    push->setFlat(true);
    push->setAutoDefault(false);
    push->setDefault(false);
    connect(push, SIGNAL(clicked(bool)), SLOT(actionClicked()));
    mActionButton.insert(type, push);
    mActionLayout->addWidget(push);
}

void TableWidget::actionClicked()
{
    auto button = static_cast<QPushButton*>(QObject::sender());
    int type = mActionButton.key(button);
    switch(type) {
    case Refresh:
        emit tableRefreshed();
        break;
    case Add:
        emit addClicked();
        break;
    case Update:
        emit updateClicked(mTableView->currentIndex());
        break;
    case Delete:
        emit deleteClicked(mTableView->currentIndex());
        break;
    }
}
