/*
 * datesettingdialog.cpp
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
#include "datesettingdialog.h"
#include "ui_datesettingdialog.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QProcess>

using namespace LibGUI;

DateSettingDialog::DateSettingDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DateSettingDialog) {
    ui->setupUi(this);
    QDateTime dt = QDateTime::currentDateTime();
    if (dt.date().year() < 2000)
        dt = dt.fromString("2000-01-01 07:00:00", "yyyy-MM-dd hh:mm:ss");
    ui->dateTimeEdit->setDateTime(dt);
    connect(ui->pushButton, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

DateSettingDialog::~DateSettingDialog() { delete ui; }

void DateSettingDialog::closeEvent(QCloseEvent *event) {
    if (!mIsOk)
        event->ignore();
    QDialog::closeEvent(event);
}

void DateSettingDialog::saveClicked() {
    QProcess p;
    p.start("date", QStringList{"-s", ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")});
    p.waitForFinished();
    if (p.exitCode() != 0) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to set the date time"));
        return;
    }
    auto curDate = QDate::currentDate();
    if (curDate.year() < 2017) {
        QMessageBox::critical(this, tr("Error"), tr("Seems your date is not up to date. Please check again"));
        return;
    }
    mIsOk = true;
#ifdef Q_PROCESSOR_ARM
    p.start("hwclock", QStringList("-w"));
    p.waitForFinished();
#endif
    close();
}
