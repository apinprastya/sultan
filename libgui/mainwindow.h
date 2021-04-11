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

#include "abstractsultangui.h"
#include "gui_global.h"
#include "messagehandler.h"
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class QLabel;

namespace LibG {
class MessageBus;
}

namespace LibGUI {

class StatusBarWidget;
class Splash;
class LoginDialog;

class GUISHARED_EXPORT MainWindow : public QMainWindow, public LibG::MessageHandler, public LibG::AbstractSultanGUI {
    Q_OBJECT

  public:
    MainWindow(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~MainWindow();
    void setup();
    void showSplashScreen() override;
    void hideSplashScreen() override;
    void splashShowMessage(const QString &msg) override;
    void showSetting() override;
    void showMainWindow() override;
    void showRestartError(const QString &title, const QString &msg) override;
    void guiMessage(int id, const QString &str) override;
    void setSettingSocketOpenClose(std::function<void(const QString &, int)> openCon,
                                   std::function<void()> closeCon) override;

  protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void messageReceived(LibG::Message *msg) override;

  private:
    Ui::MainWindow *ui;
    int mLastIdLogin = 0;
    StatusBarWidget *mStatusBar;
    Splash *mSplashUi;
    LoginDialog *mLoginDialog;
    QNetworkAccessManager mNam;

    void setupConnection();

  private slots:
    void loginSuccess();
    void logout();
    void showWindowFullScreen();
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
    void openExportImport();
    void openCheckStock();
    void openInitialStock();
    void openUnit();
    void openDateSetting();
    void resetDatabase();
    void openStockCard();
    void openSoldReturn();
    void httpRequestDone(QNetworkReply *reply);
};

} // namespace LibGUI
#endif // MAINWINDOW_H
