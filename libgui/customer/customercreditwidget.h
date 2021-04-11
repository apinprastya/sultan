/*
 * customercredit.h
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
#ifndef CUSTOMERCREDIT_H
#define CUSTOMERCREDIT_H

#include "messagehandler.h"
#include <QWidget>

namespace Ui {
class NormalWidget;
class CreditSummaryWidget;
} // namespace Ui

namespace LibGUI {

class TableWidget;
class AddCreditPaymentDialog;

class CustomerCreditWidget : public QWidget, public LibG::MessageHandler {
    Q_OBJECT
  public:
    CustomerCreditWidget(int id, const QString &number, LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~CustomerCreditWidget();
    inline int getId() { return mId; }

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::NormalWidget *ui;
    Ui::CreditSummaryWidget *uiSummary;
    TableWidget *mTableWidget;
    int mId;
    AddCreditPaymentDialog *mAddDialog;
    double mTotal;

  private slots:
    void addClicked();
    void refreshCustomer();
    void printClicked();
    void print(const QVariantMap &data);
};

} // namespace LibGUI
#endif // CUSTOMERCREDIT_H
