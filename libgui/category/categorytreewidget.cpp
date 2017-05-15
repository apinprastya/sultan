/*
 * categorytreewidget.cpp
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
#include "categorytreewidget.h"
#include <QDebug>

using namespace LibGUI;

CategoryTreeWidget::CategoryTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    setHeaderLabels(QStringList() << tr("Name") << tr("Code"));
    setColumnWidth(0, 250);
}

CategoryTreeWidget::~CategoryTreeWidget()
{
}

void CategoryTreeWidget::load(const QVariantList &data)
{
    clear();
    mItems.clear();
    for(const QVariant &d : data) {
        const QVariantMap &m = d.toMap();
        int parent = m["parent_id"].toInt();
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << m["name"].toString() << m["code"].toString());
        item->setData(0, Qt::UserRole, m["id"].toInt());
        if(parent > 0) {
            auto p = getItemWithId(parent);
            if(p != nullptr) {
                p->addChild(item);
            }
        }
        mItems.append(item);
    }
    for(auto item : mItems) {
        if(item->parent() == nullptr) {
            addTopLevelItem(item);
        }
    }
    expandAll();
}

QTreeWidgetItem *CategoryTreeWidget::getItemWithId(int id)
{
    for(auto item : mItems) {
        if(item->data(0, Qt::UserRole).toInt() == id) {
            return item;
        }
    }
    return nullptr;
}
