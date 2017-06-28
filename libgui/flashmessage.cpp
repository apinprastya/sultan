/*
 * flashmessage.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "flashmessage.h"
#include <QTimer>
#include <QMap>
#include <QLabel>
#include <QVBoxLayout>
#include <QStringBuilder>
#include <QDebug>

using namespace LibGUI;

const QMap<int, int> LIFETIME{{FlashMessage::Short, 2000}, {FlashMessage::Long, 5000}};

FlashMessage::FlashMessage(const QString &message, int type, int lifetime, QWidget *parent) :
    QWidget(parent),
    mTimer(new QTimer(this))
{
    setMinimumWidth(300);
    setMaximumWidth(300);
    auto layout = new QVBoxLayout();
    auto label = new QLabel(message, this);
    layout->setMargin(0);
    label->setWordWrap(true);
    layout->addWidget(label);
    setLayout(layout);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    QString style = "border: 2px solid #ababab;border-radius: 10px;";
    switch(type) {
    case Info:
        style = style % "background-color: white;";
        break;
    case Warning:
        style = style % "background-color: yellow;";
        break;
    case Error:
        style = style % "background-color: red;";
        break;
    }
    setStyleSheet(style);
    mTimer->setSingleShot(true);
    mTimer->start(LIFETIME[lifetime]);
    connect(mTimer, SIGNAL(timeout()), SLOT(lifetimeDone()));
    QTimer::singleShot(1, this, SLOT(show()));
    adjustSize();
}

FlashMessage::~FlashMessage()
{
}

void FlashMessage::mouseReleaseEvent(QMouseEvent *event)
{
    lifetimeDone();
    QWidget::mouseReleaseEvent(event);
}

void FlashMessage::lifetimeDone()
{
    if(mTimer->isActive()) mTimer->stop();
    emit done(this);
    close();
    deleteLater();
}
