/*
 * categorytreewidget.cpp
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
            if(p != nullptr)
                p->addChild(item);
            else
                continue;
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

QTreeWidgetItem *CategoryTreeWidget::addItem(const QVariantMap &data)
{
    int parent = data["parent_id"].toInt();
    auto p = getItemWithId(parent);
    auto item = new QTreeWidgetItem(QStringList() << data["name"].toString() << data["code"].toString());
    item->setData(0, Qt::UserRole, data["id"].toInt());
    if(p != nullptr)
        p->addChild(item);
    else
        addTopLevelItem(item);
    mItems.append(item);
    return item;
}

void CategoryTreeWidget::updateItem(const QVariantMap &data)
{
    int id = data["id"].toInt();
    auto item = getItemWithId(id);
    if(item != nullptr) {
        item->setData(0, Qt::DisplayRole, data["name"]);
        item->setData(1, Qt::DisplayRole, data["code"]);
    }
}

void CategoryTreeWidget::deleteItem(int id)
{
    auto item = getItemWithId(id);
    if(item != nullptr) {
        if(item->parent() != nullptr) {
            item->parent()->removeChild(item);
        }
        cleanChild(item);
        delete item;
    }
}

QList<CategoryData> CategoryTreeWidget::getData()
{
    QList<CategoryData> retVal;
    for(int i = 0; i < topLevelItemCount(); i++) {
        populateData(topLevelItem(i), retVal, 0);
    }
    return retVal;
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

void CategoryTreeWidget::populateData(QTreeWidgetItem *item, QList<CategoryData> &data, int pos)
{
    const QString &origName = item->data(0, Qt::DisplayRole).toString();
    const QString &name = origName.rightJustified(pos * 4 + origName.length(), ' ');
    CategoryData d(item->data(0, Qt::UserRole).toInt(), 0, name,
                   item->data(1, Qt::DisplayRole).toString());
    data.append(d);
    for(int i = 0; i < item->childCount(); i++) {
        if(item->childCount() > 0) {
            populateData(item->child(i), data, pos + 1);
        } else {
            const QString &origN = item->child(i)->data(0, Qt::DisplayRole).toString();
            const QString &n = origN.rightJustified((pos + 1) * 4 + origN.length(), ' ');
            CategoryData d1(item->child(i)->data(0, Qt::UserRole).toInt(), 0,
                            n, item->child(i)->data(1, Qt::DisplayRole).toString());
            data.append(d1);
        }
    }
}

void CategoryTreeWidget::cleanChild(QTreeWidgetItem *item)
{
    mItems.removeOne(item);
    for(auto i : mItems) {
        if(i->parent() == item)
            cleanChild(i);
    }
}
