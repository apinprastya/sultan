/*
 * categorytreewidget.h
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
#ifndef CATEGORYTREEWIDGET_H
#define CATEGORYTREEWIDGET_H

#include <QTreeWidget>

namespace LibGUI {

class CategoryTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CategoryTreeWidget(QWidget *parent = nullptr);
    ~CategoryTreeWidget();
    void load(const QVariantList &data);

private:
    QList<QTreeWidgetItem*> mItems;
    QTreeWidgetItem *getItemWithId(int id);
};

}
#endif // CATEGORYTREEWIDGET_H
