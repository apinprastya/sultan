/*
 * tabview.h
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
#ifndef TABVIEW_H
#define TABVIEW_H

#include "gui_global.h"
#include <QTabWidget>
#include <functional>

class QLabel;

namespace LibGUI {

class GUISHARED_EXPORT TabWidget : public QTabWidget
{
    Q_OBJECT
    public:
        TabWidget(QWidget *parent = nullptr);
        void tbnAddTab(QWidget *widget, const QString &name);
        void tbnRemoveTab(int index);
        bool isTabAvailable(std::function<bool(QWidget *)> func);
        void closeAllTabAndFree();

    private:
        QLabel *mLabel;
};

}

#endif // TABVIEW_H
