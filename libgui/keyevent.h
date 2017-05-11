/*
 * keyevent.h
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
#ifndef KEYEVENT_H
#define KEYEVENT_H

#include "gui_global.h"
#include <QObject>

class QKeyEvent;

namespace LibGUI {

class GUISHARED_EXPORT KeyEvent : public QObject
{
    Q_OBJECT
public:
    KeyEvent(QObject *parent = 0);
    void discardKey(Qt::Key key);
    void addConsumeKey(Qt::Key key);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QList<Qt::Key> mDiscardKey;
    QList<Qt::Key> mConsumeKey;

signals:
    void keyPressed(QObject *sender, QKeyEvent *keyEvent);
};

}
#endif // KEYEVENT_H
