/*
 * categoryadddialg.cpp
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
#include "categotyadddialog.h"
#include "guiutil.h"
#include "ui_categoryadddialog.h"
#include "util.h"
#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;

CategoryAddDialog::CategoryAddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CategoryAddDialog) {
    ui->setupUi(this);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

CategoryAddDialog::~CategoryAddDialog() { delete ui; }

void CategoryAddDialog::reset() {
    ui->lineCode->clear();
    ui->lineName->clear();
    ui->comboParent->setFocus(Qt::TabFocusReason);
    ui->comboParent->setDisabled(false);
    ui->pushSave->setEnabled(true);
    mId = -1;
}

void CategoryAddDialog::fill(int id, int parent, const QString &name, const QString &code) {
    for (int i = 0; i < ui->comboParent->count(); i++) {
        if (parent == ui->comboParent->itemData(i).toInt()) {
            ui->comboParent->setCurrentIndex(i);
            break;
        }
    }
    ui->lineName->setText(name);
    ui->lineCode->setText(code);
    ui->lineName->setFocus(Qt::TabFocusReason);
    ui->comboParent->setDisabled(true);
    ui->pushSave->setEnabled(true);
    mId = id;
}

void CategoryAddDialog::enableSaveButton(bool enable) { ui->pushSave->setEnabled(enable); }

QComboBox *CategoryAddDialog::getComboParent() { return ui->comboParent; }

void CategoryAddDialog::saveClicked() {
    if (GuiUtil::anyEmpty(QList<QWidget *>() << ui->lineName << ui->lineCode)) {
        QMessageBox::warning(this, tr("Error"), tr("Please fill all form"));
        return;
    }
    QVariantMap data;
    data.insert("name", LibG::Util::capitalize(ui->lineName->text()));
    data.insert("code", LibG::Util::capitalize(ui->lineCode->text()));
    if (ui->comboParent->isEnabled())
        data.insert("parent_id", ui->comboParent->currentData().toInt());
    ui->pushSave->setEnabled(false);
    emit saveRequest(data, mId);
}
