/*
 * addtransactiondialog.h
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
#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AddTransactionDialog;
}

namespace LibGUI {

class AddTransactionDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    AddTransactionDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AddTransactionDialog();
    void reset();
    void fill(const QVariantMap &data);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::AddTransactionDialog *ui;
    int mId = 0;
    int mBankId = 0;

private slots:
    void saveClicked();
};

}
#endif // ADDTRANSACTIONDIALOG_H
