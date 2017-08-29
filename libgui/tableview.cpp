/*
 * tableview.cpp
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
#include "tableview.h"
#include "horizontalheader.h"
#include <QScrollBar>

using namespace LibGUI;

TableView::TableView(QWidget *parent, bool useStandartHeader) :
    QTableView(parent)
{
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    mHeader = new HorizontalHeader(this, useStandartHeader);
    setHorizontalHeader(mHeader);
    setTabKeyNavigation(false);
    verticalScrollBar()->setTracking(false);
}

HeaderWidget *TableView::getHeaderWidget(int index)
{
    return mHeader->getHeaderWidget(index);
}

void TableView::setUseStandardHeader(bool value)
{
    mHeader->setUseStandardHeader(value);
}

void TableView::scrollContentsBy(int dx, int dy)
{
    QTableView::scrollContentsBy(dx, dy);
    if(dx != 0)
        mHeader->fixWidgetPositions();
}
