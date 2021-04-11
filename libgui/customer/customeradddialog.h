/*
 * customeradddialog.h
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
#ifndef CUSTOMERADDDIALOG_H
#define CUSTOMERADDDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class CustomerAddDialog;
}

namespace LibGUI {

class CustomerAddDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    CustomerAddDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~CustomerAddDialog();
    void reset();
    void fill(const QVariantMap &data);

  protected:
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::CustomerAddDialog *ui;
    int mId = 0;

  signals:
    void customerAdded();
    void customerUpdated(int id);

  private slots:
    void saveClicked();
};

} // namespace LibGUI
#endif // CUSTOMERADDDIALOG_H
