/*
 * transactionlistdialog.h
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
#ifndef TRANSACTIONLISTDIALOG_H
#define TRANSACTIONLISTDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <functional>

namespace Ui {
class TransactionListDialog;
}

namespace LibGUI {

class TransactionListDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    TransactionListDialog(LibG::MessageBus *bus, QWidget *parent = 0);
    ~TransactionListDialog();
    inline void setPrintFunction(std::function<void(QVariantMap)> func) { mPrintFunction = func; }

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::TransactionListDialog *ui;
    std::function<void(QVariantMap)> mPrintFunction;

private slots:
    void focusAndSelectTable();
    void printTransaction();
};

}
#endif // TRANSACTIONLISTDIALOG_H
