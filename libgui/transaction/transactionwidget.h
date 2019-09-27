/*
 * transactionwidget.h
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
#ifndef TRANSACTIONWIDGET_H
#define TRANSACTIONWIDGET_H

#include "messagehandler.h"
#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;
class TileWidget;

class TransactionWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    TransactionWidget(LibG::MessageBus *bus, QWidget *parent = nullptr);

protected:
    void messageReceived(LibG::Message *msg) override;
    void showEvent(QShowEvent *e) override;

private:
    Ui::NormalWidget *ui;
    TableWidget *mTableWidget;
    TileWidget *mTileIncome;
    TileWidget *mTileExpense;
    TileWidget *mTileDifference;
    TileWidget *mTileNet;
    TileWidget *mTileProfit;
    bool isShowed = false;

private slots:
    void refreshSummary();
    void addClicked();
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndexList &index);
    void exportClicked();
};

}
#endif // TRANSACTIONWIDGET_H
