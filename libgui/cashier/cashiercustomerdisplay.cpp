/*
 * Copyright 2021, Apin <apin.klas@gmail.com>
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

#include "cashiercustomerdisplay.h"
#include "cashieritem.h"
#include "cashiertablemodel.h"
#include "customerdisplay.h"
#include "customerdisplaymanager.h"
#include "global_setting_const.h"
#include "preference.h"
#include <QLocale>

using namespace LibGUI;
using namespace LibG;

static CashierCustomerDisplay *sInstance = nullptr;

void CashierCustomerDisplay::createInstance(QObject *parent) {
    if (sInstance == nullptr)
        sInstance = new CashierCustomerDisplay(parent);
}

CashierCustomerDisplay *CashierCustomerDisplay::instance() { return sInstance; }

void CashierCustomerDisplay::setCashierModel(CashierTableModel *model) {
    if (mCashierModel != nullptr) {
        mCashierModel->disconnect(this);
    }
    mCashierModel = model;
    if (mCashierModel != nullptr) {
        connect(mCashierModel, SIGNAL(itemInserted(CashierItem *)), SLOT(itemInserted(CashierItem *)));
    }
}

CashierCustomerDisplay::CashierCustomerDisplay(QObject *parent) : QObject(parent) {
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), SLOT(timeout()));
    mTimer.start(2000);
}

void CashierCustomerDisplay::checkState() {
    if (mCurrentState == Item) {
        showTotal();
    } else if (mCurrentState == Payment || mCurrentState == None) {
        showNone();
    }
}

void CashierCustomerDisplay::showTotal() {
    LibPrint::CustomerDisplay disp;
    disp.clear().writeString(tr("Total"), Preference::formatMoney(mCashierModel->getTotal()));
    writeData(disp.data());
    mCurrentState = Total;
}

void CashierCustomerDisplay::showNone() {
    auto welcome1 = Preference::getString(SETTING::CUSDISPLAY_WELCOME1);
    auto welcome2 = Preference::getString(SETTING::CUSDISPLAY_WELCOME2);
    LibPrint::CustomerDisplay disp;
    disp.clear().writeString(welcome1);
    disp.bottom().left().writeString(welcome2);
    writeData(disp.data());
    mCurrentState = None;
}

void CashierCustomerDisplay::showTotalAndChange(double total, double change) {
    mCurrentState = Payment;
    LibPrint::CustomerDisplay disp;
    disp.clear().writeString(tr("Total"), Preference::formatMoney(total));
    disp.bottom().left().writeString(tr("Change"), Preference::formatMoney(change));
    writeData(disp.data());
    mTimer.start(300000);
}

void CashierCustomerDisplay::writeData(const QByteArray &data) {
    auto cdm = LibPrint::CustomerDisplayManager::instance();
    if (cdm->open()) {
        cdm->write(data);
    }
}

QString CashierCustomerDisplay::getCount(const QString &count) {
    if (count.endsWith("00")) {
        return count.left(count.length() - 3);
    }
    return count;
}

void CashierCustomerDisplay::timeout() { checkState(); }

void CashierCustomerDisplay::itemInserted(CashierItem *item) {
    LibPrint::CustomerDisplay disp;
    disp.clear().writeString(item->name);
    disp.bottom().writeString(
        QString("%1x%2").arg(getCount(Preference::formatFloat(item->count))).arg(Preference::formatMoney(item->price)),
        Preference::formatMoney(item->total));
    writeData(disp.data());
    mTimer.stop();
    mTimer.start(5000);
    mCurrentState = Item;
}
