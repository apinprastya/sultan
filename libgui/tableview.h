/*
 * tableview.h
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
#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "gui_global.h"
#include <QTableView>

namespace LibGUI {

class HorizontalHeader;
class HeaderWidget;

class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent = nullptr, bool useStandartHeader = false);
    HeaderWidget *getHeaderWidget(int index);
    void setUseStandardHeader(bool value);

protected:
    void scrollContentsBy(int dx, int dy) override;

private:
    HorizontalHeader *mHeader;
};

}
#endif // TABLEVIEW_H
