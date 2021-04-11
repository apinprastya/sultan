/*
 * statusbarwidget.cpp
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
#include "statusbarwidget.h"
#include "ui_statusbarwidget.h"
#include "usersession.h"
#include <QDateTime>
#include <QStringBuilder>
#include <QTimer>

using namespace LibGUI;

StatusBarWidget::StatusBarWidget(QWidget *parent) : QWidget(parent), ui(new Ui::StatusBarWidget) {
    ui->setupUi(this);
    updateClock();
}

StatusBarWidget::~StatusBarWidget() { delete ui; }

void StatusBarWidget::updateUser() {
    ui->labelUser->setText(LibG::UserSession::username() % " - " % LibG::UserSession::name());
}

void StatusBarWidget::updateClock() {
    ui->labelClock->setText(QDateTime::currentDateTime().toString("dd-MMMM-yyyy hh:mm:ss"));
    QTimer::singleShot(500, this, SLOT(updateClock()));
}
