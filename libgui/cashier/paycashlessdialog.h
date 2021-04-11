/*
 * paycashlessdialog.h
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
#ifndef PAYCASHLESSDIALOG_H
#define PAYCASHLESSDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QMap>

namespace Ui {
class PayCashlessDialog;
}

namespace LibGUI {

class PayCashlessDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    PayCashlessDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~PayCashlessDialog();
    void showDialog(const double &total);
    int getBank();
    int getCardType();
    QString getCardNumber();

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::PayCashlessDialog *ui;
    double mTotal = 0;
    double mAdditonal = 0;
    QMap<int, std::tuple<QString, QString>> mAdditionalCharge;

  private slots:
    void calculateTotal();
    void payClicked();

  signals:
    void requestPay(int type, double payment, int flag);
};

} // namespace LibGUI
#endif // PAYCASHLESSDIALOG_H
