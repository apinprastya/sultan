/*
 * additemdialog.h
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
#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QModelIndexList>

namespace Ui {
class AddItemDialog;
}

namespace LibGUI {

class AddItemDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    enum Tab { Price, Package, Ingridient, ItemLink };
    AddItemDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~AddItemDialog();
    void reset(bool isAddAgain = false);
    void openBarcode(const QString &barcode);
    void setAsUpdate();
    void disableAddAgain();
    void setBarcode(const QString &barcode);
    inline bool isSuccess() { return mIsSuccess; }

  protected:
    void messageReceived(LibG::Message *msg) override;
    void showEvent(QShowEvent *event) override;
    void fill(const QVariantMap &data);

  private:
    Ui::AddItemDialog *ui;
    bool mIsUpdate = false;
    bool mIsAddAgain = false;
    int mCurrentSuplier = 0;
    int mCUrrentCategory = 0;
    bool mIsOk = false;
    bool mIsReturnPressed = false;
    bool mIsSuccess = false;
    int mBarcodeLinkRequest = -1;
    int mBarcodeOtherLinkRequest = -1;
    double mPackBuyPrice;
    bool mIsCloseAfter = false;
    bool mIsFromSave = false;
    QString mCurrentUnit;
    QVariantList mPriceList;
    QString mCurrentBarcode;

    void saveData();
    int getItemFlagFromCheckbox();
    void applyItemFlagToCheckbox(int flag);

  private slots:
    void barcodeDone();
    void returnPressed();
    void saveClicked();
    void saveAndAgainClicked();
    void saveCloseClicked();
    void checkWidget();
    void calculateDiscount();
    void addPriceClicked();
    void updatePriceClicked(const QModelIndex &index);
    void deletePriceClicked(const QModelIndexList &index);
    void openSearchItem();
    void getItemPrice();
    double updatePackagePrice();
    void tableItemLinkDoubleClicked();
    void addIngridient();
    void updateIngridient(const QModelIndex &index);
    void deleteIngridient(const QModelIndexList &index);
    void calculateIngridientPrice();

  signals:
    void success();
};

} // namespace LibGUI
#endif // ADDITEMDIALOG_H
