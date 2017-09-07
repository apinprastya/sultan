/*
 * tablewidget.h
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
#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "gui_global.h"
#include <QWidget>
#include <QMap>

class QHBoxLayout;
class QPushButton;

namespace LibGUI {

class TableView;
class TableModel;

class TableWidget : public QWidget
{
    Q_OBJECT
public:
    enum ButtonType {
        Refresh, Add, Delete, Update, Unknown
    };

    TableWidget(QWidget *parent = 0, bool useStandartHeader = false);
    ~TableWidget();
    void initButton(const QList<ButtonType> buttons);
    void initCrudButton();
    void addActionButton(QPushButton *button);
    void setupTable();
    inline TableModel *getModel() { return mModel; }
    inline TableView *getTableView() { return mTableView; }
    QPushButton *addActionButton(const QIcon &icon);
    inline void addEnableNoSelect(QPushButton *btn) { mEnableNoSelect.append(btn); }
    inline void setEnableDoubleClickUpdate(bool value) { mEnableDoubleClickUpdate = value; }

private:
    TableView *mTableView;
    TableModel *mModel;
    QHBoxLayout *mActionLayout;
    QMap<int, QPushButton*> mActionButton;
    QList<QPushButton*> mEnableNoSelect;
    bool mEnableDoubleClickUpdate = true;

    QPushButton *addActionButton(const QString &path, int type);
    QPushButton *addActionButton(const QIcon &icon, int type);
    void enableNoSelect(bool value);

signals:
    void tableRefreshed();
    void addClicked();
    void updateClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);

private slots:
    void actionClicked();
    void tableSelected();
    void tableDoubleClicked(const QModelIndex &index);
};

}
#endif // TABLEWIDGET_H
