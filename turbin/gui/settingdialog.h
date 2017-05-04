/*
 * settingdialog.h
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
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
    void showDialog();

private:
    Ui::SettingDialog *ui;
    bool mMysqlOk;
    bool mConOk;

    void saveMysqlSetting();

private slots:
    void checkSetting();
    void checkType();
    void checkMysql();
    void checkConnection();
    void cancel();
    void save();
};

#endif // SETTINGDIALOG_H
