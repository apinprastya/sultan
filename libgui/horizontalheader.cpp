/*
 * horizontalheader.cpp
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
#include "horizontalheader.h"
#include "headerwidget.h"
#include "tablemodel.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDebug>
#include <QLineEdit>

#define HEADER_HEGHT_NORMAL 30
#define HEADER_HEGHT 60

using namespace LibGUI;

HorizontalHeader::HorizontalHeader(QWidget *parent, bool useStandartHeader)
    : QHeaderView(Qt::Horizontal, parent), mUseStandartHeader(useStandartHeader) {
    connect(this, SIGNAL(sectionResized(int, int, int)), SLOT(handleSectionResized(int)));
    connect(this, SIGNAL(sectionMoved(int, int, int)), SLOT(handleSectionMoved(int, int, int)));
}

void HorizontalHeader::showEvent(QShowEvent *e) {
    for (int i = 0; i < count(); i++) {
        int type = model()->headerData(i, Qt::Horizontal, TableModel::FilterRole).toInt();
        const QString &title = model()->headerData(i, Qt::Horizontal, TableModel::TitleRole).toString();
        if (!mBoxes[i]) {
            auto *box = new HeaderWidget(i, type, title, this);
            mBoxes[i] = box;
            connect(box, SIGNAL(filterValueChanged(int, QVariant)), SIGNAL(filterValueChanged(int, QVariant)));
            fillDefValue(i);
        }
        mBoxes[i]->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());
        mBoxes[i]->show();
    }
    QHeaderView::showEvent(e);
}

QSize HorizontalHeader::sizeHint() const {
    QSize baseSize = QHeaderView::sizeHint();
    baseSize.setHeight(mUseStandartHeader ? HEADER_HEGHT_NORMAL : HEADER_HEGHT);
    return baseSize;
}

void HorizontalHeader::fixWidgetPositions() {
    for (int i = 0; i < mBoxes.size(); i++)
        mBoxes[i]->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());
}

HeaderWidget *HorizontalHeader::getHeaderWidget(int index) {
    if (index >= 0 && mBoxes.count() > index)
        return static_cast<HeaderWidget *>(mBoxes[index]);
    return nullptr;
}

void HorizontalHeader::fillDefValue(int index) {
    auto widget = getHeaderWidget(index);
    const QVariant &d = model()->headerData(index, Qt::Horizontal, TableModel::FilterValueRole);
    int type = model()->headerData(index, Qt::Horizontal, TableModel::FilterRole).toInt();
    if (d.isValid()) {
        if (type == HeaderWidget::LineEdit) {
            widget->getLineEdit()->blockSignals(true);
            widget->getLineEdit()->setText(d.toString());
            widget->getLineEdit()->blockSignals(false);
            emit filterValueChanged(index, d);
        } else if (type == HeaderWidget::Date) {
            widget->getDateEdit()->blockSignals(true);
            widget->getDateEdit()->setDate(d.toDate());
            widget->getDateEdit()->blockSignals(false);
            emit filterValueChanged(index, d);
        } else if (type == HeaderWidget::DateStartEnd) {
            widget->getDateEdit()->blockSignals(true);
            widget->getDateEnd()->blockSignals(true);
            widget->getDateEdit()->setDate(d.toMap()["start"].toDate());
            widget->getDateEnd()->setDate(d.toMap()["end"].toDate());
            widget->getDateEdit()->blockSignals(false);
            widget->getDateEnd()->blockSignals(false);
            emit filterValueChanged(index, d);
        }
    }
}

void HorizontalHeader::handleSectionResized(int i) {
    if (mBoxes.isEmpty())
        return;
    for (int j = visualIndex(i); j < count(); j++) {
        int logical = logicalIndex(j);
        mBoxes[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
    }
}

void HorizontalHeader::handleSectionMoved(int /*logical*/, int oldVisualIndex, int newVisualIndex) {
    for (int i = qMin(oldVisualIndex, newVisualIndex); i < count(); i++) {
        int logical = logicalIndex(i);
        mBoxes[logical]->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
    }
}
