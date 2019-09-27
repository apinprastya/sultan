/*
 * purchaseitemwidget.h
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
#ifndef PURCHASEITEMWIDGET_H
#define PURCHASEITEMWIDGET_H

#include "messagehandler.h"
#include <QWidget>
#include <QModelIndexList>

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class TableWidget;
class PurchaseAddItemDialog;
class TileWidget;

class PurchaseItemWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT

public:
    PurchaseItemWidget(const QVariantMap &data, LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~PurchaseItemWidget();
    inline int getId() { return mId; }

private:
    Ui::NormalWidget *ui;
    int mId;
    TableWidget *mTableWidget;
    PurchaseAddItemDialog *mAddDialog;
    TileWidget *mTileTotal;
    TileWidget *mTileDiscount;
    TileWidget *mTileFinal;

protected:
    void messageReceived(LibG::Message *msg) override;

private slots:
    void addClicked();
    void updateClicked(const QModelIndex &index);
    void delClicked(const QModelIndexList &index);
    void refreshSummary();
};

}
#endif // PURCHASEITEMWIDGET_H
