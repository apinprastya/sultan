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
#include "item/itemwidget.h"
#include "usersession.h"
#include "global_constant.h"
#include <QShortcut>
#include <QDateTime>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>

using namespace LibGUI;
using namespace LibG;

MainWindow::MainWindow(LibG::MessageBus *bus, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mMessageBus(bus),
    mLabelTime(new QLabel(this))
{
    ui->setupUi(this);
    statusBar()->addWidget(mLabelTime);
    ui->tabWidget->clear();
    setupConnection();
    updateClock();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    if(mLastIdLogin != UserSession::id())
        ui->tabWidget->closeAllTabAndFree();
    ui->action_User->setEnabled(UserSession::hasPermission(PERMISSION::USER));
    ui->action_Category->setEnabled(UserSession::hasPermission(PERMISSION::CATEGORY));
    ui->action_Suplier->setEnabled(UserSession::hasPermission(PERMISSION::SUPLIER));
    ui->action_Cashier->setEnabled(UserSession::hasPermission(PERMISSION::CASHIER));
    ui->action_Purchase->setEnabled(UserSession::hasPermission(PERMISSION::PURCASHE));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit logout();
    event->ignore();
}

void MainWindow::setupConnection()
{
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this, SLOT(closeCurrentTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(nextTab()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this, SLOT(prevTab()));
    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_C), this, SLOT(openCashier()));
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
    connect(ui->action_Purchase, SIGNAL(triggered(bool)), SLOT(openPurchase()));
}

void MainWindow::updateClock()
{
    /*auto dt = QDateTime::currentDateTime();
    mLabelTime->setText(dt.toString("dd-MMM-yyyy hh:mm:ss"));
    QTimer::singleShot(1000, this, SLOT(updateClock()));*/
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
        ui->tabWidget->tbnAddTab(new SettingWidget(this), tr("Setting"));
}

void MainWindow::openUser()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<UserWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new UserWidget(mMessageBus, this), tr("User"));
}

void MainWindow::openSuplier()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<SuplierWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new SuplierWidget(mMessageBus, this), tr("Suplier"));
}

void MainWindow::openCashier()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<CashierWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new CashierWidget(mMessageBus, this), tr("Cahsier"));
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
        ui->tabWidget->tbnAddTab(new CategoryWidget(mMessageBus, this), tr("Category"));
}

void MainWindow::openItem()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<ItemWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new ItemWidget(mMessageBus, this), tr("Item"));
}

void MainWindow::openPurchase()
{
    if(!ui->tabWidget->isTabAvailable([](QWidget* widget) -> bool {
        return (dynamic_cast<PurchaseWidget*>(widget) != nullptr);
    }))
        ui->tabWidget->tbnAddTab(new PurchaseWidget(mMessageBus, this), tr("Purcase"));
}
