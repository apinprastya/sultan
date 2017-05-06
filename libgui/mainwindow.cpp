/*
 * mainwindow.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "ui/settingwidget.h"
#include "ui/userwidget.h"
#include <QShortcut>

using namespace LibGUI;

MainWindow::MainWindow(LibG::MessageBus *bus, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mMessageBus(bus)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    setupConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnection()
{
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this, SLOT(closeCurrentTab()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(ui->actionSetting, SIGNAL(triggered(bool)), SLOT(openSetting()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->actionLogout, SIGNAL(triggered(bool)), SIGNAL(logout()));
    connect(ui->action_User, SIGNAL(triggered(bool)), SLOT(openUser()));
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

void MainWindow::openSetting()
{
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<SettingWidget*>(ui->tabWidget->widget(i));
        if(widget != nullptr) {
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(widget));
            return;
        }
    }
    ui->tabWidget->tbnAddTab(new SettingWidget(this), tr("Setting"));
}

void MainWindow::openUser()
{
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        auto widget = dynamic_cast<UserWidget*>(ui->tabWidget->widget(i));
        if(widget != nullptr) {
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(widget));
            return;
        }
    }
    ui->tabWidget->tbnAddTab(new UserWidget(this), tr("User"));
}
