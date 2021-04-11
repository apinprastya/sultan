/*
 * addingridientdialog.h
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
#ifndef ADDINGRIDIENTDIALOG_H
#define ADDINGRIDIENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariantMap>

namespace Ui {
class AddIngridientDialog;
}

namespace LibGUI {

class AddIngridientDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    AddIngridientDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AddIngridientDialog();
    void setData(const QVariantMap &data);
    inline QVariantMap &getData() { return mData; }
    inline bool isOk() { return mIsOk; }

  protected:
    void messageReceived(LibG::Message *msg) override;
    void fill();

  private:
    Ui::AddIngridientDialog *ui;
    double mBuyPrice;
    double mSellPrice;
    bool mIsOk = false;
    QVariantMap mData;

  private slots:
    void barcodeClicked();
    void saveClicked();
    void calculatePrice();
};

} // namespace LibGUI
#endif // ADDINGRIDIENTDIALOG_H
