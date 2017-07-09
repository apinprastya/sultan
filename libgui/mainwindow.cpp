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
#include "ui_mainwindow.h"
#include "messagebus.h"
#include "setting/settingwidget.h"
#include "user/userwidget.h"
#include "suplier/suplierwidget.h"
#include "about/aboutdialog.h"
#include "cashier/cashierwidget.h"
#include "category/categorywidget.h"
#include "purchase/purchasewidget.h"
#include "purchase/purchaseitemwidget.h"
#include "item/itemwidget.h"
#include "report/saleswidget.h"
#include "report/reportitemwidget.h"
#include "statusbar/statusbarwidget.h"
#include "customer/customerwidget.h"
#include "customer/customercreditwidget.h"
#include "customer/customerrewardwidget.h"
#include "machine/machinewidget.h"
#include "customer/rewardwidget.h"
#include "bank/bankwidget.h"
#include "transaction/transactionwidget.h"
#include "transaction/moneywidget.h"
#include "about/autoupdatedialog.h"
#include "usersession.h"
#include "global_constant.h"
#include "printer.h"
#include "user/changepassworddialog.h"
#include "flashmessagemanager.h"
#include "purchase/purchasereturnwidget.h"
#include <QShortcut>
#include <QDateTime>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

MainWindow::MainWindow(LibG::MessageBus *bus, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mMessageBus(bus)
{
    ui->setupUi(this);
    mStatusBar = new StatusBarWidget(this);
    statusBar()->addPermanentWidget(mStatusBar, 1);
    ui->tabWidget->clear();
    setupConnection();
    LibPrint::Printer::instance();
#ifdef FLASHMESSAGE_USE_PARENT
    FlashMessageManager::setParent(this);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    LibPrint::Printer::destroy();
}

void MainWindow::setup()
{
    ui->tabWidget->closeAllTabAndFree();
    ui->actionSetting->setEnabled(UserSession::hasPermission(PERMISSION::ADMINISTRATOR));
    ui->action_User->setEnabled(UserSession::hasPermission(PERMISSION::USER));
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
    ui->action_Cashier->setShortcut(Qt::CTRL + Qt::Key_D);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit logout();
    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event)
{
    mStatusBar->updateUser();
    QMainWindow::showEvent(event);
}

void MainWindow::setupConnection()
{
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showWindowFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this, SLOT(closeCurrentTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(nextTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this, SLOT(prevTab()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(ui->actionSetting, SIGNAL(triggered(bool)), SLOT(openSetting()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->action_About, SIGNAL(triggered(bool)), SLOT(openAbout()));
    connect(ui->actionLogout, SIGNAL(triggered(bool)), SIGNAL(logout()));
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
}

void MainWindow::showWindowFullScreen()
{
    if(isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::closeTab(int index)
{
    auto widget = ui->tabWidget->widget(index);
    ui->tabWidget->tbnRemoveTab(index);
    widget->deleteLater();
}

void MainWindow::closeCurrentTab()
{
    if(ui->tabWidget->count() > 0)
        closeTab(ui->tabWidget->currentIndex());
}

void MainWindow::nextTab()
{
    if(ui->tabWidget->count() > 0) {
        int curIndex = ui->tabWidget->currentIndex() + 1;
        if(curIndex >= ui->tabWidget->count()) curIndex = 0;
        ui->tabWidget->setCurrentIndex(curIndex);
    }
}

void MainWindow::prevTab()
{
    if(ui->tabWidget->count() > 0) {
        int curIndex = ui->tabWidget->currentIndex() - 1;
        if(curIndex < 0) curIndex = ui->tabWidget->count() - 1;
        ui->tabWidget->setCurrentIndex(curIndex);
    }
}

void MainWindow::openSetting()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<SettingWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new SettingWidget(mMessageBus, this), tr("Setting"), ":/images/16x16/gear.png");
}

void MainWindow::openUser()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<UserWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new UserWidget(mMessageBus, this), tr("User"), ":/images/16x16/user.png");
}

void MainWindow::openSuplier()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<SuplierWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new SuplierWidget(mMessageBus, this), tr("Suplier"), ":/images/16x16/truck.png");
}

void MainWindow::openCashier()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<CashierWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new CashierWidget(mMessageBus, this), tr("Cahsier"), ":/images/16x16/computer-arrow.png");
}

void MainWindow::openAbout()
{
    AboutDialog about(this);
    about.exec();
}

void MainWindow::openCategory()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<CategoryWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new CategoryWidget(mMessageBus, this), tr("Category"), ":/images/16x16/folder-open.png");
}

void MainWindow::openItem()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<ItemWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new ItemWidget(mMessageBus, this), tr("Item"), ":/images/16x16/odata.png");
}

void MainWindow::openPurchase()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<PurchaseWidget*>(widget) != nullptr);
    })) {
        auto widget = new PurchaseWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Purcase"), ":/images/16x16/baggage-cart.png");
        connect(widget, SIGNAL(requestOpenPurchaseWidget(QVariantMap)), SLOT(openPurchaseItem(QVariantMap)));
    }
}

void MainWindow::openPurchaseItem(const QVariantMap &data)
{
    int id = data["id"].toInt();
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<PurchaseItemWidget*>(ui->tabWidget->widget(i));
        if(widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new PurchaseItemWidget(data, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Pur : %1").arg(data["number"].toString()), ":/images/16x16/bagbox.png");
}

void MainWindow::openSalesReport()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<SalesWidget*>(widget) != nullptr);
    })) {
        auto widget = new SalesWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Sales"), ":/images/16x16/document-attribute-r.png");
    }
}

void MainWindow::openItemReport()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<ReportItemWidget*>(widget) != nullptr);
    })) {
        auto widget = new ReportItemWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Items Sales"), ":/images/16x16/document-attribute-r.png");
    }
}

void MainWindow::openChangePassword()
{
    ChangePasswordDialog dialog(mMessageBus);
    dialog.exec();
}

void MainWindow::openCustomer()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<CustomerWidget*>(widget) != nullptr);
    })) {
        auto widget = new CustomerWidget(mMessageBus, this);
        connect(widget, SIGNAL(requestOpenCustomerCredit(int,QString)), SLOT(openCustomerCredit(int,QString)));
        connect(widget, SIGNAL(requestOpenCustomerReward(int,QString)), SLOT(openCustomerReward(int,QString)));
        ui->tabWidget->tbnAddTab(widget, tr("Customers"), ":/images/16x16/users.png");
    }
}

void MainWindow::openCustomerCredit(int id, const QString &number)
{
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<CustomerCreditWidget*>(ui->tabWidget->widget(i));
        if(widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new CustomerCreditWidget(id, number, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Credit : %1").arg(number), ":/images/16x16/money-arrow.png");
}

void MainWindow::openCustomerReward(int id, const QString &number)
{
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<CustomerRewardWidget*>(ui->tabWidget->widget(i));
        if(widget != nullptr && widget->getId() == id) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    auto w = new CustomerRewardWidget(id, number, mMessageBus, this);
    ui->tabWidget->tbnAddTab(w, tr("Reward : %1").arg(number), ":/images/16x16/rosette-label.png");
}

void MainWindow::openMachine()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<MachineWidget*>(widget) != nullptr);
    })) {
        auto widget = new MachineWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Machine"), ":/images/16x16/computer-arrow.png");
    }
}

void MainWindow::openReward()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<RewardWidget*>(widget) != nullptr);
    })) {
        auto widget = new RewardWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Reward"), ":/images/16x16/rosette-label.png");
    }
}

void MainWindow::openBank()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<BankWidget*>(widget) != nullptr);
    })) {
        auto widget = new BankWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Bank"), ":/images/16x16/building.png");
    }
}

void MainWindow::openTransaction()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<TransactionWidget*>(widget) != nullptr);
    })) {
        auto widget = new TransactionWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Transaction"), ":/images/16x16/report.png");
    }
}

void MainWindow::openMoney()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<MoneyWidget*>(widget) != nullptr);
    })) {
        auto widget = new MoneyWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Money"), ":/images/16x16/report.png");
    }
}

void MainWindow::openAutoUpdate()
{
    AutoUpdateDialog dialog(this);
    dialog.exec();
}

void MainWindow::openPurchaseReturn()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<PurchaseReturnWidget*>(widget) != nullptr);
    })) {
        auto widget = new PurchaseReturnWidget(mMessageBus, this);
        ui->tabWidget->tbnAddTab(widget, tr("Purchase Return"), ":/images/16x16/bagbox.png");
    }
}
