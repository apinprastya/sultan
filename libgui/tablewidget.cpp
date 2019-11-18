/*
 * tablewidget.cpp
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
#include "tablewidget.h"
#include "tableview.h"
#include "tablemodel.h"
#include "horizontalheader.h"
#include "paginationwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QDebug>

using namespace LibGUI;

static QMap<int, QString> BTNICONS{
    {TableWidget::Refresh, QStringLiteral(":/images/16x16/refresh.png")},
    {TableWidget::Add, QStringLiteral(":/images/16x16/plus.png")},
    {TableWidget::Update, QStringLiteral(":/images/16x16/pencil.png")},
    {TableWidget::Delete, QStringLiteral(":/images/16x16/cross.png")}
};

static QMap<int, QString> BTNTOOLTIP{
    {TableWidget::Refresh, QStringLiteral("Refresh")},
    {TableWidget::Add, QStringLiteral("Add")},
    {TableWidget::Update, QStringLiteral("Update")},
    {TableWidget::Delete, QStringLiteral("Delete")}
};

TableWidget::TableWidget(QWidget *parent, bool useStandartHeader) :
    QWidget(parent),
    mTableView(new TableView(this, useStandartHeader)),
    mModel(new TableModel(this, useStandartHeader)),
    mPaginationWidget(new PaginationWidget(this))
{
    auto mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->addWidget(mTableView);
    mActionLayout = new QHBoxLayout();
    mActionLayout->addWidget(mPaginationWidget);
    mActionLayout->addStretch();
    mainLayout->addLayout(mActionLayout);
    setLayout(mainLayout);
    mTableView->setModel(mModel);
    mTableView->verticalHeader()->hide();
    mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    auto header = static_cast<HorizontalHeader*>(mTableView->horizontalHeader());
    connect(header, SIGNAL(filterValueChanged(int,QVariant)), mModel, SLOT(filterChanged(int,QVariant)));
    connect(mTableView, SIGNAL(clicked(QModelIndex)), SLOT(tableSelected()));
    connect(mTableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(tableDoubleClicked(QModelIndex)));
    connect(mPaginationWidget, SIGNAL(pageChanged(int)), mModel, SLOT(loadPage(int)));
    connect(mModel, SIGNAL(maxPageChanged(int)), mPaginationWidget, SLOT(setMaxPage(int)));
    connect(mModel, SIGNAL(currentPageChanged(int)), mPaginationWidget, SLOT(setCurrentPage(int)));
    connect(mPaginationWidget, &PaginationWidget::perPageChanged, mModel, &TableModel::setPerPageCount);
}

TableWidget::~TableWidget()
{
}

void TableWidget::initButton(const QList<ButtonType> buttons)
{
    for(int i : buttons) {
        if(BTNICONS.contains(i)) {
            auto push = addActionButton(BTNICONS[i], i);
            if(i == Delete || i == Update) {
                mEnableNoSelect.append(push);
            }
            push->setToolTip(BTNTOOLTIP[i]);
        }
    }
    enableNoSelect(false);
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

QPushButton *TableWidget::addActionButton(const QIcon &icon)
{
    return addActionButton(icon, Unknown);
}

void TableWidget::setDefaultPerPage(int index)
{
    static QMap<int, int> pageMap{{0, 10}, {1, 25}, {2, 50}, {3, 100}};
    mModel->setPerPageCount(pageMap[index]);
    mPaginationWidget->setCurrentPerPage(index, true);
}

QPushButton *TableWidget::addActionButton(const QString &path, int type)
{
    return addActionButton(QIcon(path), type);
}

QPushButton *TableWidget::addActionButton(const QIcon &icon, int type)
{
    if(type != Unknown && mActionButton.contains(type)) return mActionButton[type];
    auto push = new QPushButton(this);
    push->setIcon(icon);
    push->setFlat(true);
    push->setAutoDefault(false);
    push->setDefault(false);
    if(type != Unknown) {
        connect(push, SIGNAL(clicked(bool)), SLOT(actionClicked()));
        mActionButton.insert(type, push);
    }
    mActionLayout->addWidget(push);
    return push;
}

void TableWidget::enableNoSelect(bool value)
{
    for(auto btn : mEnableNoSelect)
        btn->setEnabled(value);
}

void TableWidget::actionClicked()
{
    auto button = static_cast<QPushButton*>(QObject::sender());
    int type = mActionButton.key(button);
    switch(type) {
    case Refresh:
        mModel->refresh();
        enableNoSelect(false);
        emit tableRefreshed();
        break;
    case Add:
        emit addClicked();
        break;
    case Update:
        if(mTableView->currentIndex().isValid())
            emit updateClicked(mTableView->currentIndex());
        break;
    case Delete:
        /*if(mTableView->currentIndex().isValid())
            emit deleteClicked(mTableView->currentIndex());*/
        if(!mTableView->selectionModel()->selectedRows().empty()) {
            emit deleteClicked(mTableView->selectionModel()->selectedRows());
        }
        break;
    }
}

void TableWidget::tableSelected()
{
    enableNoSelect(true);
}

void TableWidget::tableDoubleClicked(const QModelIndex &index)
{
    auto sender = qobject_cast<TableView*>(QObject::sender());
    if(sender != nullptr && !mEnableDoubleClickUpdate) return;
    if(index.isValid())
        emit updateClicked(index);
}
