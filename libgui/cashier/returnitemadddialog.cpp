/*
 * returnitemadddialog.cpp
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
#include "returnitemadddialog.h"
#include "ui_returnitemadddialog.h"
#include "preference.h"
#include "cashiertablemodel.h"
#include "cashieritem.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

ReturnItemAddDialog::ReturnItemAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReturnItemAddDialog)
{
    ui->setupUi(this);
    connect(ui->doubleCount, SIGNAL(valueChanged(double)), SLOT(calculateTotal()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

ReturnItemAddDialog::~ReturnItemAddDialog()
{
    delete ui;
}

void ReturnItemAddDialog::fill(const QVariantMap &d)
{
    mId = d["id"].toInt();
    mPrice = d["price"].toDouble();
    mDiscount = d["discount"].toDouble();
    mFlag = d["flag"].toInt();
    mBuyPrice = d["buy_price"].toDouble();
    ui->labelBarcode->setText(d["barcode"].toString());
    ui->labelName->setText(d["name"].toString());
    ui->labelPrice->setText(Preference::formatMoney(mPrice));
    ui->labelDiscount->setText(Preference::formatMoney(mDiscount));
    ui->doubleCount->setMaximum(d["count"].toDouble());
    ui->doubleCount->setValue(d["count"].toDouble());
    ui->doubleCount->selectAll();
    mUnit = d["unit"].toString();
    calculateTotal();
}

void ReturnItemAddDialog::calculateTotal()
{
    ui->labelTotal->setText(Preference::formatMoney((mPrice - mDiscount) * ui->doubleCount->value()));
}

void ReturnItemAddDialog::saveClicked()
{
    auto model = CashierTableModel::instance();
    auto item = model->getItemWithFlag(ui->labelBarcode->text(), CashierItem::Return);
    if(item != nullptr) {
        QMessageBox::critical(this, tr("Error"), tr("Return item with barcode already exists"));
        return;
    }
    auto it = model->addReturnItem(ui->doubleCount->value(), ui->labelName->text(), ui->labelBarcode->text(), mPrice, mDiscount, mUnit, mFlag | CashierItem::Return);
    it->linkId = mId;
    it->buyPrice = mBuyPrice;
    close();
}
