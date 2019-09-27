/*
 * AddItemUnavailableDialog.cpp
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

#ifndef ADDITEMUNAVAILABLEDIALOG_H
#define ADDITEMUNAVAILABLEDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AddItemUnavailableDialog;
}

namespace LibGUI {

class AddItemUnavailableDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    AddItemUnavailableDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AddItemUnavailableDialog();
    void openBarcode(const QString &barcode);
    void openAutoBarcode();
    inline bool isOk() { return mIsOk; }
    inline bool isAuto() { return mIsAutoBarcode; }

private:
    Ui::AddItemUnavailableDialog *ui;
    bool mIsOk = false;
    bool mIsAutoBarcode = false;

    void fill(const QString &barcode);

protected:
    void messageReceived(LibG::Message *msg) override;

private slots:
    void saveClicked();
    void sellPriceChanged();

signals:
    void addNewItem(const QVariantMap &data);
};

}

#endif // ADDITEMUNAVAILABLEDIALOG_H
