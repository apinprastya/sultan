/*
 * returnadddialog.h
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
#ifndef RETURNADDDIALOG_H
#define RETURNADDDIALOG_H

#include "messagehandler.h"
#include "purchaseitem.h"
#include <QDialog>

namespace Ui {
class ReturnAddDialog;
}

namespace LibGUI {

class ReturnAddDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    ReturnAddDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~ReturnAddDialog();
    void reset();
    void fill(const QVariantMap &data);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::ReturnAddDialog *ui;
    int mId = -1;
    int mCurrentSuplier = -1;
    PurchaseItem mItem;
    bool mIsAddAgain = false;

    void populateSuplier(const QVariantList &list);
    void fillField();

private slots:
    void purchaseItemFocused();
    void saveClicked();
    void calculateTotal();
};

}
#endif // RETURNADDDIALOG_H
