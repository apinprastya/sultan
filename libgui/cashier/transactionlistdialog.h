/*
 * transactionlistdialog.h
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
#ifndef TRANSACTIONLISTDIALOG_H
#define TRANSACTIONLISTDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariantMap>
#include <functional>

namespace Ui {
class TransactionListDialog;
}

namespace LibGUI {

class TransactionListDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    enum DialogType { Cashier, SoldReturn };
    TransactionListDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~TransactionListDialog();
    void setType(int type);
    inline void setPrintFunction(std::function<void(QVariantMap)> func) { mPrintFunction = func; }
    inline bool isOk() { return mIsOk; }
    inline QVariantMap getData() { return mData; }

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::TransactionListDialog *ui;
    int mType = Cashier;
    bool mIsOk = false;
    std::function<void(QVariantMap)> mPrintFunction;
    QVariantMap mData;

private slots:
    void focusAndSelectTable();
    void printTransaction();
    void search();
    void showItems();
};

}
#endif // TRANSACTIONLISTDIALOG_H
