/*
 * solditemlistdialog.h
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
#ifndef SOLDITEMLISTDIALOG_H
#define SOLDITEMLISTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class SoldItemListDialog;
}

namespace LibGUI {

class SoldItemListDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    SoldItemListDialog(const QVariantMap &data, LibG::MessageBus *bus, QWidget *parent = 0);
    ~SoldItemListDialog();

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    Ui::SoldItemListDialog *ui;
    int mId;

private slots:
    void focusAndSelectTable();
    void tableReturnPressed();
};

}
#endif // SOLDITEMLISTDIALOG_H
