/*
 * keyevent.cpp
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
#include "keyevent.h"
#include <QEvent>
#include <QKeyEvent>

using namespace LibGUI;

KeyEvent::KeyEvent(QObject *parent) : QObject(parent)
{

}

void KeyEvent::discardKey(Qt::Key key)
{
    mDiscardKey.append(key);
}

void KeyEvent::addConsumeKey(Qt::Key key)
{
    mConsumeKey.append(key);
}

bool KeyEvent::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        for(int i = 0; i < mDiscardKey.size(); i++) {
            if(mDiscardKey[i] == keyEvent->key())
                return true;
        }
        for(int i = 0; i < mConsumeKey.size(); i++) {
            if(mConsumeKey[i] == keyEvent->key()) {
                emit keyPressed(obj, keyEvent);
                return true;
            }
        }
    } else if(mFocusEvent && event->type() == QEvent::FocusIn) {
        emit focused(obj);
        return true;
    }
    return QObject::eventFilter(obj, event);
}
