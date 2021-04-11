/*
 * searchcustomerdialog.h
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
#ifndef SEARCHCUSTOMERDIALOG_H
#define SEARCHCUSTOMERDIALOG_H

#include <QDialog>
#include <QVariantMap>

namespace Ui {
class SearchCustomerDialog;
}

namespace LibG {
class MessageBus;
}

namespace LibGUI {

class SearchCustomerDialog : public QDialog {
    Q_OBJECT

  public:
    SearchCustomerDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~SearchCustomerDialog();
    inline QVariantMap getSelectedData() { return mSelectedData; }
    inline bool isOk() { return mIsOk; }

  private:
    Ui::SearchCustomerDialog *ui;
    QVariantMap mSelectedData;
    bool mIsOk = false;

  private slots:
    void nameDone();
    void dataLoaded();
    void returnPressed();
    void doubleClicked(const QModelIndex &index);
};

} // namespace LibGUI

#endif // SEARCHCUSTOMERDIALOG_H
