/*
 * checkstockadddialog.h
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

#ifndef CHECKSTOCKADDDIALOG_H
#define CHECKSTOCKADDDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class CheckStockAddDialog;
}

namespace LibGUI {

class CheckStockAddDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    CheckStockAddDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~CheckStockAddDialog();
    void reset();

  protected:
    void messageReceived(LibG::Message *msg);

  private:
    Ui::CheckStockAddDialog *ui;
    bool mIsAddAgain = false;
    float mLastStock = 0;
    double mBuyPrice = 0;
    QString mLastBarcode;

    void inputCheckStock();

  private slots:
    void barcodeDone();
    void addClicked();
    void addAgainClicked();
    void checkDiff();
    void openSearchItem();
};

} // namespace LibGUI
#endif // CHECKSTOCKADDDIALOG_H
