/*
 * advancepaymentdialog.h
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
#ifndef ADVANCEPAYMENTDIALOG_H
#define ADVANCEPAYMENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class AdvancePaymentDialog;
}

namespace LibGUI {

class Customer;

class AdvancePaymentDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    AdvancePaymentDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AdvancePaymentDialog();
    void setup(double total, Customer *cust);

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::AdvancePaymentDialog *ui;
    double mTotal = 0;
    Customer *mCustomer = nullptr;

  signals:
    void payRequested(int type, double value, int flag);

  private slots:
    void paymentValueChanged(const QString &value);
    void payClicked();
};

} // namespace LibGUI
#endif // ADVANCEPAYMENTDIALOG_H
