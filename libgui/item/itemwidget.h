/*
 * itemwidget.h
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
#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "messagehandler.h"
#include <QModelIndexList>
#include <QWidget>

namespace Ui {
class ItemWidget;
}

namespace LibGUI {

class TableWidget;
class TableItem;
class AddItemDialog;
class AddPriceDialog;
class TileWidget;

class ItemWidget : public QWidget, public LibG::MessageHandler {
    Q_OBJECT
  public:
    ItemWidget(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~ItemWidget();

  protected:
    void messageReceived(LibG::Message *msg);

  private:
    Ui::ItemWidget *ui;
    TableWidget *mMainTable;
    AddItemDialog *mAddDialog;
    AddPriceDialog *mPriceDialog;
    double mCurrentBuyPrice;
    TileWidget *mStockValue;
    QString mCurrentBarcode;
    QString mCurrentName;

  private slots:
    void mainTableSelectionChanges();
    void addItemClicked();
    void updateItemClicked(const QModelIndex &index);
    void deleteItemClicked(const QModelIndexList &index);
    void importClicked();
    void exportClicked();
    void mainTableKeyPressed(QObject *, QKeyEvent *event);
    void openStockCard();
    void reloadSummary();

  private:
    void printPrice(TableItem *item);
};

} // namespace LibGUI
#endif // ITEMWIDGET_H
