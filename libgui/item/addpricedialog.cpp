/*
 * addpricedialog.cpp
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
#include "addpricedialog.h"
#include "ui_addpricedialog.h"
#include "global_constant.h"
#include "preference.h"
#include "global_setting_const.h"
#include "message.h"
#include "util.h"
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

AddPriceDialog::AddPriceDialog(bool local, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPriceDialog),
    mIsLocal(local)
{
    ui->setupUi(this);
    ui->doublePrice->setDecimals(Preference::getInt(SETTING::LOCALE_DECIMAL));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
    connect(ui->doublePrice, SIGNAL(valueChanged(double)), SLOT(updateDiscount()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), SLOT(updateDiscount()));
}

AddPriceDialog::~AddPriceDialog()
{
    delete ui;
}

void AddPriceDialog::reset()
{
    ui->doubleCount->setValue(0);
    ui->doublePrice->setValue(0);
    ui->pushSave->setEnabled(true);
    ui->doubleCount->setFocus(Qt::TabFocusReason);
    ui->lineEdit->clear();
    mId = 0;
}

void AddPriceDialog::fill(const QVariantMap &data)
{
    mId = data["id"].toInt();
    ui->doubleCount->setValue(data["count"].toDouble());
    ui->doublePrice->setValue(data["price"].toDouble());
    ui->doubleCount->setFocus(Qt::TabFocusReason);
    const QString &discFormula = data["discount_formula"].toString();
    ui->lineEdit->setText(discFormula);
    if(discFormula.isEmpty()) updateDiscount();
}

void AddPriceDialog::setBuyPrice(double buyprice)
{
    mBuyPrice = buyprice;
    ui->labelBuyPrice->setText(Preference::formatMoney(buyprice));
    updateDiscount();
}

void AddPriceDialog::saveClicked()
{
    const double &count = ui->doubleCount->value();
    const double &price = ui->doublePrice->value();
    if(count == 0 && price == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Make sure value is greater than 0"));
        return;
    }
    mData.clear();
    double discount = Util::calculateDiscount(ui->lineEdit->text(), ui->doublePrice->value());
    mData.insert("count", count);
    mData.insert("price", price);
    mData.insert("discount_formula", ui->lineEdit->text());
    mData.insert("discount", discount);
    mData.insert("final", ui->doublePrice->value() - discount);
    mIsOk = true;
    close();
}

void AddPriceDialog::updateDiscount()
{
    double discount = Util::calculateDiscount(ui->lineEdit->text(), ui->doublePrice->value());
    double margin = ui->doublePrice->value() - discount - mBuyPrice;
    double perc = 100;
    if(mBuyPrice > 0) perc = margin * 100 / mBuyPrice;
    ui->labelDiscount->setText(Preference::formatMoney(discount));
    ui->labelFinal->setText(Preference::formatMoney(ui->doublePrice->value() - discount));
    ui->labelMargin->setText(QString("%1 (%2%)").arg(Preference::formatMoney(margin)).arg(Preference::formatMoney(perc)));
}
