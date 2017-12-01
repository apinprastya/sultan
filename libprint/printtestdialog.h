/*
 * printestdialog.h
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
#ifndef PRINTTESTDIALOG_H
#define PRINTTESTDIALOG_H

#include "print_global.h"
#include "usb.h"
#include <QDialog>

namespace Ui {
class PrintTestDialog;
}

namespace LibPrint {

class PRINTSHARED_EXPORT PrintTestDialog : public QDialog
{
    Q_OBJECT

public:
    PrintTestDialog(QWidget *parent = 0);
    ~PrintTestDialog();

private:
    Ui::PrintTestDialog *ui;
    QList<UsbDevice> mUsbDevices;

private slots:
    void printClicked();
    void updateEnable();
    void openDrawer();
};

}
#endif // PRINTTESTDIALOG_H
