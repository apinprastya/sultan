/*
 * guiutil.cpp
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
#include "guiutil.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "preference.h"
#include "printer.h"
#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTableView>
#include <QtConcurrent>

using namespace LibGUI;
using namespace LibG;

bool GuiUtil::anyEmpty(const QList<QWidget *> &lists) {
    for (auto w : lists) {
        auto line = qobject_cast<QLineEdit *>(w);
        if (line != nullptr && line->text().isEmpty())
            return true;
        auto combo = qobject_cast<QComboBox *>(w);
        if (combo != nullptr && combo->currentData().toInt() <= 0)
            return true;
        auto plain = qobject_cast<QPlainTextEdit *>(w);
        if (plain != nullptr && plain->toPlainText().isEmpty())
            return true;
        auto spin = qobject_cast<QSpinBox *>(w);
        if (spin != nullptr && spin->value() == 0)
            return true;
    }
    return false;
}

void GuiUtil::setColumnWidth(QTableView *table, const QList<int> &widths) {
    for (int i = 0; i < widths.size(); i++)
        table->setColumnWidth(i, widths[i]);
}

QString GuiUtil::toHtml(QString value) { return value.replace("\n", "<br>"); }

void GuiUtil::selectCombo(QComboBox *combo, const QVariant &value, const QString &key) {
    if (!value.isValid())
        return;
    for (int i = 0; i < combo->count(); i++) {
        if (key.isEmpty()) {
            if (combo->itemData(i) == value) {
                combo->setCurrentIndex(i);
                return;
            }
        } else {
            if (combo->itemData(i).toMap()[key] == value) {
                combo->setCurrentIndex(i);
                return;
            }
        }
    }
}

void GuiUtil::selectComboByText(QComboBox *combo, const QString &value) {
    for (int i = 0; i < combo->count(); i++) {
        if (!combo->itemText(i).compare(value, Qt::CaseInsensitive)) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

void GuiUtil::clearAll(const QList<QWidget *> &lists) {
    for (auto w : lists) {
        auto line = qobject_cast<QLineEdit *>(w);
        if (line != nullptr)
            line->clear();
        auto combo = qobject_cast<QComboBox *>(w);
        if (combo != nullptr)
            combo->setCurrentIndex(0);
        auto plain = qobject_cast<QPlainTextEdit *>(w);
        if (plain != nullptr)
            plain->clear();
        auto ds = qobject_cast<QDoubleSpinBox *>(w);
        if (ds != nullptr)
            ds->setValue(0);
        auto s = qobject_cast<QSpinBox *>(w);
        if (s != nullptr)
            s->setValue(0);
        auto l = qobject_cast<QLabel *>(w);
        if (l != nullptr)
            l->clear();
    }
}

void GuiUtil::enableWidget(bool enable, const QList<QWidget *> &lists) {
    for (auto w : lists) {
        w->setEnabled(enable);
    }
}

bool GuiUtil::isWidgetFocused(QWidget *widget) { return qApp->focusWidget() == widget; }

void GuiUtil::populateCombo(QComboBox *combo, const QVariantList &list, const QString &holder) {
    combo->clear();
    combo->addItem(holder, -1);
    for (auto &d : list) {
        const QVariantMap &m = d.toMap();
        combo->addItem(m["name"].toString(), m["id"].toInt());
    }
}

void GuiUtil::print(const QString &data) {
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    const QString &prName = Preference::getString(SETTING::PRINTER_CASHIER_NAME);
    const QString &prDevice = Preference::getString(SETTING::PRINTER_CASHIER_DEVICE);
    uint16_t vendorId = (uint16_t)Preference::getInt(SETTING::PRINTER_CASHIER_VENDOR_ID);
    uint16_t produckId = (uint16_t)Preference::getInt(SETTING::PRINTER_CASHIER_PRODUK_ID);
    QtConcurrent::run(LibPrint::Printer::instance(), &LibPrint::Printer::print,
                      type == PRINT_TYPE::DEVICE ? prDevice : prName, data, type, vendorId, produckId);
    // LibPrint::Printer::instance()->print(type == PRINT_TYPE::DEVICE ? prDevice : prName, data, type, vendorId,
    // produckId);
}
