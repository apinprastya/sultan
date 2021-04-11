/*
 * addsoldreturndialog.h
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
#ifndef ADDSOLDRETURNDIALOG_H
#define ADDSOLDRETURNDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AddSoldReturnDialog;
}

namespace LibGUI {

class AddSoldReturnDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    AddSoldReturnDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AddSoldReturnDialog();
    void fill(const QVariantMap &data);

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::AddSoldReturnDialog *ui;
    int mId = -1;
    int mSoldId = -1;
    int mSoldItemId = -1;
    int mBankSelected = -1;
    double mPrice = 0;

    void fillField(const QVariantMap &data);

  private slots:
    void saveClicked();
    void calculatePrice();
    void openTransactionDialog();
    void checkBank();
};

} // namespace LibGUI

#endif // ADDSOLDRETURNDIALOG_H
