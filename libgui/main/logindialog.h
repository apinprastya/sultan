/*
 * logindialog.h
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
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "messagehandler.h"
#include <QDialog>

namespace Ui {
class LoginDialog;
}

namespace LibGUI {

class LoginDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    LoginDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~LoginDialog();
    void reset();
    void showDialog();

  private:
    Ui::LoginDialog *ui;

  protected:
    void messageReceived(LibG::Message *msg) override;
    void closeEvent(QCloseEvent *event) override;
    void reject() override;

  signals:
    void loginSuccess();

  private slots:
    void loginClicked();
    void openSetting();
};

} // namespace LibGUI
#endif // LOGINDIALOG_H
