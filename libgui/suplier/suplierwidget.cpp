/*
 * suplierwidget.cpp
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
#include "suplierwidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "global_constant.h"

using namespace LibGUI;
using namespace LibG;

SuplierWidget::SuplierWidget(MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this))
{
    ui->setupUi(this);
    ui->labelTitle->setText(tr("Suplier"));
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("name", tr("Name"));
    model->addColumn("code", tr("Code"));
    model->addColumn("address", tr("Address"));
    model->addColumn("phone", tr("Phone"));
    model->setTypeCommand(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    mTableWidget->setupTable();
    model->refresh();
}
