/*
 * purchaseadditemdialog.h
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#ifndef PURCHASEADDITEMDIALOG_H
#define PURCHASEADDITEMDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariant>

namespace Ui {
class PurchaseAddItemDialog;
}

namespace LibGUI {

class PurchaseAddItemDialog : public QDialog, public LibG::MessageHandler
{
    Q_OBJECT

public:
    PurchaseAddItemDialog(LibG::MessageBus *bus, int purchaseId, QWidget *parent = 0);
    ~PurchaseAddItemDialog();
    void reset();
    void fill(const QVariantMap &data);

protected:
    void messageReceived(LibG::Message *msg) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::PurchaseAddItemDialog *ui;
    int mPurchaseId;
    int mId = 0;
    bool mIsAgain = false;
    bool mIsClosing = false;
    double mDiscount;
    double mTotal;

    void save();

signals:
    void addSuccess();
    void updateSuccess(QVariant id);

private slots:
    void barcodeDone();
    void saveAgainClicked();
    void saveClicked();
    void calculateTotal();
    void calculateDiscount();
};

}
#endif // PURCHASEADDITEMDIALOG_H
