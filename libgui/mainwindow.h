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

class GUISHARED_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(LibG::MessageBus *bus, QWidget *parent = 0);
    ~MainWindow();

signals:
    void logout();

private:
    Ui::MainWindow *ui;
    LibG::MessageBus *mMessageBus;
    QLabel *mLabelTime;

    void setupConnection();

private slots:
    void updateClock();
    void closeTab(int index);
    void closeCurrentTab();
    void nextTab();
    void prevTab();
    void openSetting();
    void openUser();
    void openSuplier();
    void openCashier();
    void openAbout();
};

}
#endif // MAINWINDOW_H
