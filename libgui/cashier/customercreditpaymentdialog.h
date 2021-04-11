/*
 * customercreditpaymentdialog.h
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
#ifndef CUSTOMERCREDITPAYMENTDIALOG_H
#define CUSTOMERCREDITPAYMENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class CustomerCreditPaymentDialog;
}

namespace LibGUI {

class CustomerCreditPaymentDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    CustomerCreditPaymentDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~CustomerCreditPaymentDialog();

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::CustomerCreditPaymentDialog *ui;
    double mCredit = 0;
    int mId = 0;
    QString mNumber;

    void printData(const QVariantMap &d);

  private slots:
    void numberDone();
    void openSearchCustomer();
    void payClicked();
    void calculateRest();
};

} // namespace LibGUI
#endif // CUSTOMERCREDITPAYMENTDIALOG_H
