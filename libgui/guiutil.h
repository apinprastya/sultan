/*
 * guiutil.h
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
#ifndef GUIUTIL_H
#define GUIUTIL_H

#include "gui_global.h"
#include <QComboBox>
#include <QVariantMap>
#include <QWidget>

class QTableView;

namespace LibGUI {

class GuiUtil {
  public:
    static bool anyEmpty(const QList<QWidget *> &lists);
    static void setColumnWidth(QTableView *table, const QList<int> &widths);
    static QString toHtml(QString value);
    static void selectCombo(QComboBox *combo, const QVariant &value, const QString &key = "");
    static void selectComboByText(QComboBox *combo, const QString &value);
    static void clearAll(const QList<QWidget *> &lists);
    static void enableWidget(bool enable, const QList<QWidget *> &lists);
    static bool isWidgetFocused(QWidget *widget);
    static void populateCombo(QComboBox *combo, const QVariantList &list, const QString &holder);
    static void print(const QString &data);
};

} // namespace LibGUI
#endif // GUIUTIL_H
