/*
 * mainwindow.h
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_global.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QLabel;

namespace LibG {
class MessageBus;
}

namespace LibGUI {

class StatusBarWidget;

class GUISHARED_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(LibG::MessageBus *bus, QWidget *parent = 0);
    ~MainWindow();
    void setup();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void logout();

private:
    Ui::MainWindow *ui;
    LibG::MessageBus *mMessageBus;
    int mLastIdLogin = 0;
    StatusBarWidget *mStatusBar;

    void setupConnection();

private slots:
    void showWindowFullScreen();
    void closeTab(int index);
    void closeCurrentTab();
    void nextTab();
    void prevTab();
    void openSetting();
    void openUser();
    void openSuplier();
    void openCashier();
    void openAbout();
    void openCategory();
    void openItem();
    void openPurchase();
    void openPurchaseItem(const QVariantMap &data);
    void openSalesReport();
    void openItemReport();
    void openChangePassword();
    void openCustomer();
    void openCustomerCredit(int id, const QString &number);
    void openCustomerReward(int id, const QString &number);
    void openMachine();
    void openReward();
    void openBank();
    void openTransaction();
    void openMoney();
    void openAutoUpdate();
    void openPurchaseReturn();
};

}
#endif // MAINWINDOW_H
