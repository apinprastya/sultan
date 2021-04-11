/*
 * restartconfirmationdialog.cpp
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
#include "restartconfirmationdialog.h"
#include "settingdialog.h"
#include "ui_restartconfirmationdialog.h"
#include <QProcess>

using namespace LibGUI;

RestartConfirmationDialog::RestartConfirmationDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RestartConfirmationDialog) {
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    connect(ui->pushRestart, SIGNAL(clicked(bool)), SLOT(restartClicked()));
    connect(ui->pushExit, SIGNAL(clicked(bool)), SLOT(exitClicked()));
    connect(ui->pushSetting, SIGNAL(clicked(bool)), SLOT(openSetting()));
}

RestartConfirmationDialog::~RestartConfirmationDialog() { delete ui; }

void RestartConfirmationDialog::setMessage(const QString &title, const QString &msg) {
    setWindowTitle(title);
    ui->labelTitle->setText(msg);
}

void RestartConfirmationDialog::restartClicked() {
    qApp->quit();
    QStringList list;
    const QStringList &args = qApp->arguments();
    for (int i = 0; i < args.count(); i++) {
        if (i == 0)
            continue;
        list.append(args[i]);
    }
    QProcess::startDetached(qApp->arguments()[0], list);
}

void RestartConfirmationDialog::exitClicked() { qApp->quit(); }

void RestartConfirmationDialog::openSetting() {
    close();
    SettingDialog dialog;
    dialog.showDialog();
    dialog.exec();
    qApp->quit();
}
