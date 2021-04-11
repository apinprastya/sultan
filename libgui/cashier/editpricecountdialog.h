/*
 * editpricecountdialog.h
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
#ifndef EDITPRICECOUNTDIALOG_H
#define EDITPRICECOUNTDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class EditPriceCountDialog;
}

namespace LibGUI {

class EditPriceCountDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    EditPriceCountDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~EditPriceCountDialog();
    void setup(const QString &barcode, float count, double price, const QString disc, const QString &note, int flag);
    inline bool isOk() { return mIsOk; }
    float getCount();
    double getPrice();
    QString getDiscountFormula();
    QString getNote();

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::EditPriceCountDialog *ui;
    bool mIsOk = false;

  private slots:
    void updatePrice();
    void saveClicked();
};

} // namespace LibGUI
#endif // EDITPRICECOUNTDIALOG_H
