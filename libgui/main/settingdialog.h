/*
 * settingdialog.h
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
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <functional>

namespace Ui {
class SettingDialog;
}

class QTimer;

namespace LibGUI {

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();
    void showDialog();
    static void setSettingSocketOpenClose(std::function<void(const QString&, int)> openCon, std::function<void()> closeCon);
    static void guiMessage(int id, const QString &str);

private:
    Ui::SettingDialog *ui;
    bool mMysqlOk;
    bool mConOk;
    QTimer *mTimer;

    void saveMysqlSetting();

private slots:
    void databaseChanged();
    void checkSetting();
    void checkType();
    void checkMysql();
    void checkConnection();
    void cancel();
    void save();
    void openDirectorySelector();

public:
    void clientConnected();
    void clientError(const QString &err);
    void clientTimeout();
};

}
#endif // SETTINGDIALOG_H
