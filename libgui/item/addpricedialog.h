/*
 * addpricedialog.h
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
#ifndef ADDPRICEDIALOG_H
#define ADDPRICEDIALOG_H

#include "messagehandler.h"
#include <QDialog>
#include <QVariantMap>

namespace Ui {
class AddPriceDialog;
}

namespace LibGUI {

class AddPriceDialog : public QDialog
{
    Q_OBJECT

public:
    AddPriceDialog(bool local, QWidget *parent = nullptr);
    ~AddPriceDialog();
    void reset();
    void fill(const QVariantMap &data);
    void setBuyPrice(double buyprice);
    inline QVariantMap getData() const { return mData; }
    inline bool isOk() { return mIsOk; }

private:
    Ui::AddPriceDialog *ui;
    int mId = 0;
    bool mIsLocal;
    double mBuyPrice = 0;
    bool mIsOk = false;
    QVariantMap mData;

signals:
    void success();

private slots:
    void saveClicked();
    void updateDiscount();
};

}
#endif // ADDPRICEDIALOG_H
