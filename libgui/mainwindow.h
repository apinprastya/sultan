/*
 * mainwindow.h
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui_global.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

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

    void setupConnection();
};

}
#endif // MAINWINDOW_H
