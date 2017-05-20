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
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QTableView>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>

using namespace LibGUI;

bool GuiUtil::anyEmpty(const QList<QWidget *> &lists)
{
    for(auto w : lists) {
        auto line = qobject_cast<QLineEdit*>(w);
        if(line != nullptr && line->text().isEmpty())
            return true;
        auto combo = qobject_cast<QComboBox*>(w);
        if(combo != nullptr && combo->currentData().toInt() <= 0)
            return true;
        auto plain = qobject_cast<QPlainTextEdit*>(w);
        if(plain != nullptr && plain->toPlainText().isEmpty())
            return true;
    }
    return false;
}

void GuiUtil::setColumnWidth(QTableView *table, const QList<int> &widths)
{
    for(int i = 0; i < widths.size(); i++)
        table->setColumnWidth(i, widths[i]);
}

QString GuiUtil::toHtml(QString value)
{
    return value.replace("\n" , "<br>");
}

void GuiUtil::selectCombo(QComboBox *combo, const QVariant &value)
{
    if(!value.isValid()) return;
    for(int i = 0; i < combo->count(); i++) {
        if(combo->itemData(i) == value) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

void GuiUtil::clearAll(const QList<QWidget *> &lists)
{
    for(auto w : lists) {
        auto line = qobject_cast<QLineEdit*>(w);
        if(line != nullptr) line->clear();
        auto combo = qobject_cast<QComboBox*>(w);
        if(combo != nullptr) combo->setCurrentIndex(0);
        auto plain = qobject_cast<QPlainTextEdit*>(w);
        if(plain != nullptr) plain->clear();
        auto ds = qobject_cast<QDoubleSpinBox*>(w);
        if(ds != nullptr) ds->setValue(0);
        auto s = qobject_cast<QSpinBox*>(w);
        if(s != nullptr) s->setValue(0);
        auto l = qobject_cast<QLabel*>(w);
        if(l != nullptr) l->clear();
    }
}

void GuiUtil::enableWidget(bool enable, const QList<QWidget *> &lists)
{
    for(auto w : lists) {
        w->setEnabled(enable);
    }
}
