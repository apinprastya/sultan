/*
 * mainwindow.cpp
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
#include "mainwindow.h"
#include "about/aboutdialog.h"
#include "about/autoupdatedialog.h"
#include "admin/importexportdatabasedialog.h"
#include "bank/bankwidget.h"
#include "cashier/cashiercustomerdisplay.h"
#include "cashier/cashierwidget.h"
#include "cashier/customercreditpaymentdialog.h"
#include "category/categorywidget.h"
#include "checkstock/checkstockwidget.h"
#include "customer/customercreditwidget.h"
#include "customer/customerrewardwidget.h"
#include "customer/customerwidget.h"
#include "customer/rewardwidget.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "initialstock/initialstockwidget.h"
#include "item/itemwidget.h"
#include "machine/machinewidget.h"
#include "messagebus.h"
#include "preference.h"
#include "printer.h"
#include "purchase/purchaseitemwidget.h"
#include "purchase/purchasereturnwidget.h"
#include "purchase/purchasewidget.h"
#include "report/reportitemwidget.h"
#include "report/saleswidget.h"
#include "report/stockcardwidget.h"
#include "setting/settingwidget.h"
#include "soldreturn/solditemreturnwidget.h"
#include "statusbar/statusbarwidget.h"
#include "suplier/suplierwidget.h"
#include "transaction/moneywidget.h"
#include "transaction/transactionwidget.h"
#include "ui_mainwindow.h"
#include "unit/unitwidget.h"
#include "user/changepassworddialog.h"
#include "user/userwidget.h"
#include "usersession.h"
#ifdef USE_DATE_SETTING
#include "setting/datesettingdialog.h"
#endif
#include "main/logindialog.h"
#include "main/restartconfirmationdialog.h"
#include "main/settingdialog.h"
#include "main/splash.h"
#include "message.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QShortcut>
#include <QTimer>

using namespace LibGUI;
using namespace LibG;

MainWindow::MainWindow(LibG::MessageBus *bus, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mSplashUi(new Splash()), mLoginDialog(new LoginDialog(bus)) {
    ui->setupUi(this);
    setMessageBus(bus);
    mStatusBar = new StatusBarWidget(this);
    statusBar()->addPermanentWidget(mStatusBar, 1);
    ui->tabWidget->clear();
    setupConnection();
    LibPrint::Printer::instance();
#ifdef FLASHMESSAGE_USE_PARENT
    FlashMessageManager::setParent(this);
#endif
    setAlwaysListen(MSG_TYPE::BROADCAST);
    CashierCustomerDisplay::createInstance(this);
}

MainWindow::~MainWindow() {
    delete ui;
    LibPrint::Printer::destroy();
    delete mLoginDialog;
}

void MainWindow::setup() {
    ui->tabWidget->closeAllTabAndFree();
    ui->actionSetting->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_Reset_Database->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->actionImport_Export_Database->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_User->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_Category->setEnabled(UserSession::hasPermission(PERMISSION::CATEGORY));
    ui->action_Suplier->setEnabled(UserSession::hasPermission(PERMISSION::SUPLIER));
    ui->action_Cashier->setEnabled(UserSession::hasPermission(PERMISSION::CASHIER));
    ui->actionPurchase->setEnabled(UserSession::hasPermission(PERMISSION::PURCASHE));
    ui->actionPurchaseReturn->setEnabled(UserSession::hasPermission(PERMISSION::PURCASHE));
    ui->action_Items->setEnabled(UserSession::hasPermission(PERMISSION::ITEM_RW));
    ui->actionItems->setEnabled(UserSession::hasPermission(PERMISSION::REPORT));
    ui->actionSales->setEnabled(UserSession::hasPermission(PERMISSION::REPORT));
    ui->action_Banks->setEnabled(UserSession::hasPermission(PERMISSION::BANK));
    ui->action_Customer_List->setEnabled(UserSession::hasPermission(PERMISSION::CUSTOMER));
    ui->actionReward_Setting->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_MOney->setEnabled(UserSession::hasPermission(PERMISSION::REPORT_MONEY));
    ui->action_Transaction->setEnabled(UserSession::hasPermission(PERMISSION::REPORT_TRANS));
    ui->action_Machines->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_Check_Stock->setEnabled(UserSession::hasPermission(PERMISSION::CHECK_STOCK));
    ui->actionInitial_Stock->setEnabled(UserSession::hasPermission(PERMISSION::INITIAL_STOCK));
    ui->actionUnits->setEnabled(UserSession::hasPermission(PERMISSION::UNIT));
    ui->actionSold_Return->setEnabled(UserSession::hasPermission(PERMISSION::CASHIER));
    // ui->action_Stock_Card->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_Cashier->setShortcut(Qt::CTRL + Qt::Key_D);
    ui->action_Items->setShortcut(Qt::CTRL + Qt::Key_I);
#ifndef USE_DATE_SETTING
    ui->actionDate_Setting->setEnabled(false);
#endif
    ui->actionInitial_Stock->setVisible(false);
}

void MainWindow::showSplashScreen() {
    mSplashUi->refreshLogo();
    mSplashUi->show();
}

void MainWindow::hideSplashScreen() {
    mSplashUi->hide();
    mSplashUi->deleteLater();
}

void MainWindow::splashShowMessage(const QString &msg) { mSplashUi->setMessage(msg); }

void MainWindow::showSetting() {
    SettingDialog dialog;
    dialog.showDialog();
    dialog.exec();
}

void MainWindow::showMainWindow() { mLoginDialog->showDialog(); }

void MainWindow::showRestartError(const QString &title, const QString &msg) {
    RestartConfirmationDialog dialog;
    dialog.setMessage(title, msg);
    dialog.exec();
}

void MainWindow::guiMessage(int id, const QString &str) { SettingDialog::guiMessage(id, str); }

void MainWindow::setSettingSocketOpenClose(std::function<void(const QString &, int)> openCon,
                                           std::function<void()> closeCon) {
    SettingDialog::setSettingSocketOpenClose(openCon, closeCon);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    emit logout();
    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event) {
    // TODO : take care of it
    QMainWindow::showEvent(event);
    mStatusBar->updateUser();
#ifdef USE_DATE_SETTING
    auto date = QDate::currentDate();
    if (date.year() < 2017) {
        DateSettingDialog dialog;
        dialog.exec();
    }
#endif
    if (Preference::getInt(SETTING::MACHINE_ID) == 0) {
        QMessageBox::warning(this, tr("First setting"),
                             tr("Please set the machine ID on setting first before start using the application"));
        openSetting();
    } else {
        Message msg(MSG_TYPE::MACHINE, MSG_COMMAND::GET);
        msg.addData("id", Preference::getInt(SETTING::MACHINE_ID));
        sendMessage(&msg);
    }
}

void MainWindow::messageReceived(Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::DATABASE, MSG_COMMAND::RESET)) {
        if (msg->isSuccess()) {
            QMessageBox::information(this, tr("Success"), tr("Reset success, will restart application"));
            qApp->quit();
            QStringList list;
            const QStringList &args = qApp->arguments();
            for (int i = 0; i < args.count(); i++) {
                if (i == 0)
                    continue;
                list.append(args[i]);
            }
            QProcess::startDetached(qApp->arguments()[0], list);
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    } else if (msg->isTypeCommand(MSG_TYPE::MACHINE, MSG_COMMAND::GET)) {
        Preference::setValue(SETTING::MACHINE_CODE, msg->data("code"));
        Preference::setValue(SETTING::MACHINE_NAME, msg->data("name"));
    } else if (msg->isType(MSG_TYPE::BROADCAST)) {
        // TODO: update local cache setting
    }
}

void MainWindow::setupConnection() {
    connect(mLoginDialog, SIGNAL(loginSuccess()), SLOT(loginSuccess()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showWindowFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this, SLOT(closeCurrentTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(nextTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this, SLOT(prevTab()));
    connect(ui->actionSetting, SIGNAL(triggered(bool)), SLOT(openSetting()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->action_About, SIGNAL(triggered(bool)), SLOT(openAbout()));
    connect(ui->actionLogout, SIGNAL(triggered(bool)), SLOT(logout()));
    connect(ui->action_User, SIGNAL(triggered(bool)), SLOT(openUser()));
    connect(ui->action_Suplier, SIGNAL(triggered(bool)), SLOT(openSuplier()));
    connect(ui->action_Cashier, SIGNAL(triggered(bool)), SLOT(openCashier()));
    connect(ui->action_Items, SIGNAL(triggered(bool)), SLOT(openItem()));
    connect(ui->action_Category, SIGNAL(triggered(bool)), SLOT(openCategory()));
    connect(ui->actionPurchase, SIGNAL(triggered(bool)), SLOT(openPurchase()));
    connect(ui->actionSales, SIGNAL(triggered(bool)), SLOT(openSalesReport()));
    connect(ui->actionItems, SIGNAL(triggered(bool)), SLOT(openItemReport()));
    connect(ui->action_Change_Password, SIGNAL(triggered(bool)), SLOT(openChangePassword()));
    connect(ui->action_Customer_List, SIGNAL(triggered(bool)), SLOT(openCustomer()));
    connect(ui->action_Machines, SIGNAL(triggered(bool)), SLOT(openMachine()));
    connect(ui->actionReward_Setting, SIGNAL(triggered(bool)), SLOT(openReward()));
    connect(ui->action_Banks, SIGNAL(triggered(bool)), SLOT(openBank()));
    connect(ui->action_Transaction, SIGNAL(triggered(bool)), SLOT(openTransaction()));
    connect(ui->action_MOney, SIGNAL(triggered(bool)), SLOT(openMoney()));
    connect(ui->actionCheck_Update, SIGNAL(triggered(bool)), SLOT(openAutoUpdate()));
    connect(ui->actionPurchaseReturn, SIGNAL(triggered(bool)), SLOT(openPurchaseReturn()));
    connect(ui->actionImport_Export_Database, SIGNAL(triggered(bool)), SLOT(openExportImport()));
    connect(ui->action_Check_Stock, SIGNAL(triggered(bool)), SLOT(openCheckStock()));
    connect(ui->actionInitial_Stock, SIGNAL(triggered(bool)), SLOT(openInitialStock()));
    connect(ui->actionUnits, SIGNAL(triggered(bool)), SLOT(openUnit()));
    connect(ui->actionDate_Setting, SIGNAL(triggered(bool)), SLOT(openDateSetting()));
    connect(ui->action_Reset_Database, SIGNAL(triggered(bool)), SLOT(resetDatabase()));
    connect(ui->actionSold_Return, SIGNAL(triggered(bool)), SLOT(openSoldReturn()));
    // connect(ui->action_Stock_Card, SIGNAL(triggered(bool)), SLOT(openStockCard()));
    ui->actionCheck_Update->setVisible(false);
    connect(&mNam, SIGNAL(finished(QNetworkReply *)), SLOT(httpRequestDone(QNetworkReply *)));
}

void MainWindow::loginSuccess() {
    setup();
#ifdef QT_DEBUG
    setFixedSize(1024, 768);
    show();
#else
    showMaximized();
#endif
}

void MainWindow::logout() {
    hide();
    mLoginDialog->showDialog();
}

void MainWindow::showWindowFullScreen() {
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::closeCurrentTab() {
    if (ui->tabWidget->count() > 0)
        ui->tabWidget->tbnRemoveTab(ui->tabWidget->currentIndex());
}

void MainWindow::nextTab() {
    if (ui->tabWidget->count() > 0) {
        int curIndex = ui->tabWidget->currentIndex() + 1;
        if (curIndex >= ui->tabWidget->count())
            curIndex = 0;
        ui->tabWidget->setCurrentIndex(curIndex);
    }
}

void MainWindow::prevTab() {
    if (ui->tabWidget->count() > 0) {
        int curIndex = ui->tabWidget->currentIndex() - 1;
        if (curIndex < 0)
            curIndex = ui->tabWidget->count() - 1;
        ui->tabWidget->setCurrentIndex(curIndex);
    }
}

void MainWindow::openSetting() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<SettingWidget *>(widget) != nullptr); }))
        ui->tabWidget->tbnAddTab(new SettingWidget(mMessageBus, this), tr("Setting"), ":/images/16x16/gear.png");
}

void MainWindow::openUser() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<UserWidget *>(widget) != nullptr); }))
        ui->tabWidget->tbnAddTab(new UserWidget(mMessageBus, this), tr("User"), ":/images/16x16/user.png");
}

void MainWindow::openSuplier() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<SuplierWidget *>(widget) != nullptr); }))
        ui->tabWidget->tbnAddTab(new SuplierWidget(mMessageBus, this), tr("Suplier"), ":/images/16x16/truck.png");
}

void MainWindow::openCashier() {
    if (!ui->tabWidget->isTabAvailable([](QWidget *widget) -> bool {
            auto w = dynamic_cast<TabWidget *>(widget);
            if (w != nullptr) {
                if (w->getType() == TabWidget::Cashier)
                    return true;
            }
            return false;
        })) {
        auto tab = new TabWidget(TabWidget::Cashier, this);
        tab->setNewWidgetFunc([=]() -> QWidget * {
            auto w = new CashierWidget(mMessageBus, this);
            connect(w, SIGNAL(transactionDone()), tab, SLOT(tbnRemoveTab()));
            return w;
        });
        tab->newTab();
        ui->tabWidget->tbnAddTab(tab, tr("Cahsier"), ":/images/16x16/computer-arrow.png");
    }
}

void MainWindow::openAbout() {
    AboutDialog about(this);
    about.exec();
}

void MainWindow::openCategory() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<CategoryWidget *>(widget) != nullptr); }))
        ui->tabWidget->tbnAddTab(new CategoryWidget(mMessageBus, this), tr("Category"),
                                 ":/images/16x16/folder-open.png");
}

void MainWindow::openItem() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<ItemWidget *>(widget) != nullptr); }))
        ui->tabWidget->tbnAddTab(new ItemWidget(mMessageBus, this), tr("Item"), ":/images/16x16/odata.png");
}

void MainWindow::openPurchase() {
    if (!ui->tabWidget->isTabAvailable([](QWidget *widget) -> bool {
            auto w = dynamic_cast<TabWidget *>(widget);
            if (w != nullptr) {
                if (w->getType() == TabWidget::Purchase) {
                    w->setCurrentIndex(0);
                    return true;
                }
            }
            return false;
        })) {
        auto tab = new TabWidget(TabWidget::Purchase, this);
        auto widget = new PurchaseWidget(mMessageBus, tab);
        tab->tbnAddTab(widget, tr("Purchase"));
        ui->tabWidget->tbnAddTab(tab, tr("Purchase"), ":/images/16x16/baggage-cart.png");
        connect(widget, &PurchaseWidget::requestOpenPurchaseWidget, [=](const QVariantMap &data) {
            int id = data["id"].toInt();
            for (int i = 0; i < tab->count(); i++) {
                auto widget = dynamic_cast<PurchaseItemWidget *>(tab->widget(i));
                if (widget != nullptr && widget->getId() == id) {
                    tab->setCurrentIndex(i);
                    return;
                }
            }
            auto w = new PurchaseItemWidget(data, mMessageBus, this);
            tab->tbnAddTab(w, data["number"].toString());
        });
    }
}

void MainWindow::openPurchaseItem(const QVariantMap &data) {
    int id = data["id"].toInt();
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<PurchaseItemWidget *>(ui->tabWidget->widget(i));
        if (widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new PurchaseItemWidget(data, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Pur : %1").arg(data["number"].toString()), ":/images/16x16/bagbox.png");
}

void MainWindow::openSalesReport() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<SalesWidget *>(widget) != nullptr); })) {
        auto widget = new SalesWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Sales"), ":/images/16x16/document-attribute-r.png");
    }
}

void MainWindow::openItemReport() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<ReportItemWidget *>(widget) != nullptr); })) {
        auto widget = new ReportItemWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Items Sales"), ":/images/16x16/document-attribute-r.png");
    }
}

void MainWindow::openChangePassword() {
    ChangePasswordDialog dialog(mMessageBus);
    dialog.exec();
}

void MainWindow::openCustomer() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<CustomerWidget *>(widget) != nullptr); })) {
        auto widget = new CustomerWidget(mMessageBus, this);
        connect(widget, SIGNAL(requestOpenCustomerCredit(int, QString)), SLOT(openCustomerCredit(int, QString)));
        connect(widget, SIGNAL(requestOpenCustomerReward(int, QString)), SLOT(openCustomerReward(int, QString)));
        ui->tabWidget->tbnAddTab(widget, tr("Customers"), ":/images/16x16/users.png");
    }
}

void MainWindow::openCustomerCredit(int id, const QString &number) {
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<CustomerCreditWidget *>(ui->tabWidget->widget(i));
        if (widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new CustomerCreditWidget(id, number, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Credit : %1").arg(number), ":/images/16x16/money-arrow.png");
}

void MainWindow::openCustomerReward(int id, const QString &number) {
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<CustomerRewardWidget *>(ui->tabWidget->widget(i));
        if (widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new CustomerRewardWidget(id, number, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Reward : %1").arg(number), ":/images/16x16/rosette-label.png");
}

void MainWindow::openMachine() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<MachineWidget *>(widget) != nullptr); })) {
        auto widget = new MachineWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Machine"), ":/images/16x16/computer-arrow.png");
    }
}

void MainWindow::openReward() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<RewardWidget *>(widget) != nullptr); })) {
        auto widget = new RewardWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Reward"), ":/images/16x16/rosette-label.png");
    }
}

void MainWindow::openBank() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<BankWidget *>(widget) != nullptr); })) {
        auto widget = new BankWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Bank"), ":/images/16x16/building.png");
    }
}

void MainWindow::openTransaction() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<TransactionWidget *>(widget) != nullptr); })) {
        auto widget = new TransactionWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Transaction"), ":/images/16x16/report.png");
    }
}

void MainWindow::openMoney() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<MoneyWidget *>(widget) != nullptr); })) {
        auto widget = new MoneyWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Money"), ":/images/16x16/wallet.png");
    }
}

void MainWindow::openAutoUpdate() {
    AutoUpdateDialog dialog(this);
    dialog.exec();
}

void MainWindow::openPurchaseReturn() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<PurchaseReturnWidget *>(widget) != nullptr); })) {
        auto widget = new PurchaseReturnWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Purchase Return"), ":/images/16x16/wooden-arrow.png");
    }
}

void MainWindow::openExportImport() {
    ImportExportDatabaseDialog dialog(mMessageBus, this);
    dialog.exec();
}

void MainWindow::openCheckStock() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<CheckStockWidget *>(widget) != nullptr); })) {
        auto widget = new CheckStockWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Check Stock"), ":/images/16x16/bandaid.png");
    }
}

void MainWindow::openInitialStock() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<InitialStockWidget *>(widget) != nullptr); })) {
        auto widget = new InitialStockWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Initial Stock"), ":/images/16x16/bagbox.png");
    }
}

void MainWindow::openUnit() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<UnitWidget *>(widget) != nullptr); })) {
        auto widget = new UnitWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Unit"), ":/images/16x16/ruler.png");
    }
}

void MainWindow::openDateSetting() {
#ifdef USE_DATE_SETTING
    DateSettingDialog dialog;
    dialog.exec();
#endif
}

void MainWindow::resetDatabase() {
    const QString &str = QInputDialog::getText(
        this, tr("Confirmation"), tr("Are you sure to reset your database?\nPlease input your password to confirm"),
        QLineEdit::Password);
    if (!str.isEmpty()) {
        Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::RESET);
        msg.addData("user_id", UserSession::id());
        msg.addData("password", QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex()));
        sendMessage(&msg);
    }
}

void MainWindow::openStockCard() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<StockCardWidget *>(widget) != nullptr); })) {
        auto widget = new StockCardWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Stock Card"), ":/images/16x16/bagbox.png");
    }
}

void MainWindow::openSoldReturn() {
    if (!ui->tabWidget->isTabAvailable(
            [](QWidget *widget) -> bool { return (dynamic_cast<SoldItemReturnWidget *>(widget) != nullptr); })) {
        auto widget = new SoldItemReturnWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Sold Return"), ":/images/16x16/wooden-arrow.png");
    }
}

void MainWindow::httpRequestDone(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError)
        return;
    QJsonObject doc = QJsonDocument::fromJson(reply->readAll()).object();
    QJsonObject fields = doc.value("fields").toObject();
    QString version = fields.value("current").toObject().value("stringValue").toString().replace(".", "");
    QString appVersion = qApp->applicationVersion().replace(".", "");
    if (version.toInt() > appVersion.toInt())
        QMessageBox::information(
            this, tr("New Version"),
            tr("New version is available. Please check on %1")
                .arg("<a href=https://sultan.lekapin.com/download>https://sultan.lekapin.com/download</a>"));
}
