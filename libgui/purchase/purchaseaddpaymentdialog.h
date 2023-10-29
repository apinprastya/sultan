/*
 * purchaseaddpaymentdialog.h
 * Copyright 2023, Apin <apin.klas@gmail.com>
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

#ifndef PURCHASEADDPAYMENTDIALOG_H
#define PURCHASEADDPAYMENTDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariantMap>

namespace Ui {
class PurchaseAddPaymentDialog;
}

namespace LibGUI {

class PurchaseAddPaymentDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    PurchaseAddPaymentDialog(LibG::MessageBus *bus, int purchaseId, float residual, const QVariantMap &data,
                             const QVariantMap &oldData, QWidget *parent = nullptr);
    ~PurchaseAddPaymentDialog();

  private:
    Ui::PurchaseAddPaymentDialog *ui;
    int mPurchaseId = 0;
    float mResidual = 0;
    QString mNumber;
    QString mSupplier;
    QVariantMap mOldData;

  protected:
    void messageReceived(LibG::Message *msg) override;

  private slots:
    void cancelClicked();
    void saveClicked();
    void showError(const QString &error);
};

} // namespace LibGUI

#endif // PURCHASEADDPAYMENTDIALOG_H
