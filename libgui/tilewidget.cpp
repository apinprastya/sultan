/*
 * tilewidget.cpp
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
#include "tilewidget.h"
#include "ui_tilewidget.h"

using namespace LibGUI;

TileWidget::TileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TileWidget)
{
    ui->setupUi(this);
}

TileWidget::~TileWidget()
{
    delete ui;
}

void TileWidget::setTitle(const QString &title)
{
    ui->labelTitle->setText(title);
}

void TileWidget::setValue(const QString &value)
{
    ui->labelValue->setText(value);
}

void TileWidget::setTitleValue(const QString &title, const QString &value)
{
    ui->labelTitle->setText(title);
    ui->labelValue->setText(value);
}
