/*
 * splash.cpp
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
#include "splash.h"
#include "ui_splash.h"

using namespace LibGUI;

Splash::Splash(QWidget *parent) : QDialog(parent), ui(new Ui::Splash) {
    ui->setupUi(this);
    setWindowTitle("");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog);
}

Splash::~Splash() { delete ui; }

void Splash::setMessage(const QString &msg) { ui->labelLoading->setText(msg); }
