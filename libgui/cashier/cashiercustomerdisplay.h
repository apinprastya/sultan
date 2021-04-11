/*
 * Copyright 2021, Apin <apin.klas@gmail.com>
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

#ifndef CASHIERCUSTOMERDISPLAY_H
#define CASHIERCUSTOMERDISPLAY_H

#include <QObject>
#include <QTimer>

namespace LibGUI {

class CashierTableModel;
class CashierItem;

enum CustomerDisplayState {
    None,
    Item,
    Total,
    Payment,
};

class CashierCustomerDisplay : public QObject {
    Q_OBJECT
  public:
    static void createInstance(QObject *parent);
    static CashierCustomerDisplay *instance();
    void setCashierModel(CashierTableModel *model);
    void showTotal();
    void showNone();
    void showTotalAndChange(double total, double change);

  private:
    CashierCustomerDisplay(QObject *parent = nullptr);

    QTimer mTimer;
    CustomerDisplayState mCurrentState = None;
    CashierTableModel *mCashierModel = nullptr;

    void checkState();
    void writeData(const QByteArray &data);
    QString getCount(const QString &count);

  private slots:
    void timeout();
    void itemInserted(CashierItem *item);
};

} // namespace LibGUI

#endif // CASHIERCUSTOMERDISPLAY_H
