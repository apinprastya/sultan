/*
 * itemwidget.h
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
#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class ItemWidget;
}

namespace LibGUI {

class TableWidget;
class AddItemDialog;
class AddPriceDialog;

class ItemWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    ItemWidget(LibG::MessageBus *bus, QWidget *parent = 0);
    ~ItemWidget();

protected:
    void messageReceived(LibG::Message *msg);

private:
    Ui::ItemWidget *ui;
    TableWidget *mMainTable;
    TableWidget *mSecondTable;
    AddItemDialog *mAddDialog;
    AddPriceDialog *mPriceDialog;
    QString mCurrentBarcode;
    QString mCurrentName;
    double mCurrentBuyPrice;

private slots:
    void mainTableSelectionChanges();
    void addItemClicked();
    void updateItemClicked(const QModelIndex &index);
    void deleteItemClicked(const QModelIndex &index);
    void addPriceClicked();
    void updatePriceClicked(const QModelIndex &index);
    void deletePriceClicked(const QModelIndex &index);
    void importClicked();
    void exportClicked();
};

}
#endif // ITEMWIDGET_H
