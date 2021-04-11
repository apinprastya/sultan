/*
 * horizontalheader.h
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
#ifndef HORIZONTALHEADER_H
#define HORIZONTALHEADER_H

#include "gui_global.h"
#include <QHeaderView>

namespace LibGUI {

class HeaderWidget;

class GUISHARED_EXPORT HorizontalHeader : public QHeaderView {
    Q_OBJECT
  public:
    HorizontalHeader(QWidget *parent = nullptr, bool useStandartHeader = false);
    void showEvent(QShowEvent *e) override;
    QSize sizeHint() const override;
    void fixWidgetPositions();
    HeaderWidget *getHeaderWidget(int index);
    inline void setUseStandardHeader(bool val) { mUseStandartHeader = val; }

  private:
    QMap<int, QWidget *> mBoxes;
    bool mUseStandartHeader;

    void fillDefValue(int index);

  signals:
    void filterValueChanged(int index, const QVariant &value);

  private slots:
    void handleSectionResized(int i);
    void handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);
};

} // namespace LibGUI
#endif // HORIZONTALHEADER_H
