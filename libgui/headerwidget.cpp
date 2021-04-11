/*
 * headerwidget.cpp
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
#include "headerwidget.h"
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

using namespace LibGUI;

static QString STYLE{"font-size: 11px;"};

HeaderWidget::HeaderWidget(int index, int type, const QString &title, QWidget *parent)
    : QWidget(parent), mIndex(index) {
    auto lay = new QVBoxLayout();
    auto label = new QLabel(title, this);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setStyleSheet(STYLE);
    if (type != DateStartEnd)
        lay->addWidget(label);
    if (type == LineEdit) {
        mLineEdit = new QLineEdit(this);
        mLineEdit->setStyleSheet(STYLE);
        lay->addWidget(mLineEdit);
        connect(mLineEdit, SIGNAL(returnPressed()), SLOT(lineEditDone()));
    } else if (type == Combo) {
        mComboBox = new QComboBox(this);
        mComboBox->setStyleSheet(STYLE);
        lay->addWidget(mComboBox);
        connect(mComboBox, SIGNAL(currentIndexChanged(int)), SLOT(comboChanged()));
    } else if (type == Date) {
        mDateEdit = new QDateEdit(this);
        mDateEdit->setCalendarPopup(true);
        mDateEdit->setStyleSheet(STYLE);
        lay->addWidget(mDateEdit);
        connect(mDateEdit, SIGNAL(dateChanged(QDate)), SLOT(dateChanged()));
    } else if (type == DateStartEnd) {
        mDateEdit = new QDateTimeEdit(this);
        mDateEdit->setCalendarPopup(true);
        mDateEdit->setStyleSheet(STYLE);
        lay->addWidget(mDateEdit);
        connect(mDateEdit, SIGNAL(dateChanged(QDate)), SLOT(dateStartEndChanged()));
        mDateEnd = new QDateTimeEdit(this);
        mDateEnd->setCalendarPopup(true);
        mDateEnd->setStyleSheet(STYLE);
        lay->addWidget(mDateEnd);
        connect(mDateEnd, SIGNAL(dateChanged(QDate)), SLOT(dateStartEndChanged()));
    }
    setLayout(lay);
}

void HeaderWidget::lineEditDone() { emit filterValueChanged(mIndex, mLineEdit->text()); }

void HeaderWidget::comboChanged() { emit filterValueChanged(mIndex, mComboBox->currentData()); }

void HeaderWidget::dateChanged() { emit filterValueChanged(mIndex, mDateEdit->date()); }

void HeaderWidget::dateStartEndChanged() {
    QVariantMap map;
    map.insert("start", mDateEdit->dateTime());
    map.insert("end", mDateEnd->dateTime());
    emit filterValueChanged(mIndex, map);
}
