/*
 * purchaseitemselectiondialog.h
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
#ifndef PURCHASEITEMSELECTIONDIALOG_H
#define PURCHASEITEMSELECTIONDIALOG_H

#include "messagebus.h"
#include "purchaseitem.h"
#include <QDialog>

namespace Ui {
class PurchaseItemSelectionDialog;
}

namespace LibGUI {

class PurchaseItemSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    PurchaseItemSelectionDialog(LibG::MessageBus *bus, int suplier, PurchaseItem *item, QWidget *parent = 0);
    ~PurchaseItemSelectionDialog();

private:
    Ui::PurchaseItemSelectionDialog *ui;
    PurchaseItem *mItem;

private slots:
    void tableDoubleClicked(const QModelIndex &index);
};

}
#endif // PURCHASEITEMSELECTIONDIALOG_H
