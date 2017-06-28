/*
 * flashmessagemanager.cpp
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
#include "flashmessagemanager.h"
#include <QApplication>
#include <QDesktopWidget>

#define SCREEN_Y_MARGIN     20
#define SCREEN_X_MARGIN     20
#define Y_SPACE             20

using namespace LibGUI;

static FlashMessageManager *sInstance = nullptr;

void FlashMessageManager::showMessage(const QString &message, int type, int lifetime)
{
    if(sInstance == nullptr) {
        sInstance = new FlashMessageManager(qApp);
    }
    auto flash = new FlashMessage(message, type, lifetime, sInstance->mParent);
    sInstance->mFlashMessages.append(flash);
    sInstance->rearrangeMessage();
    connect(flash, SIGNAL(done(FlashMessage*)), sInstance, SLOT(flashMessageClosed(FlashMessage*)));
}

void FlashMessageManager::showError(const QString &message, int lifetime)
{
    showMessage(message, FlashMessage::Error, lifetime);
}

void FlashMessageManager::showWarning(const QString &message, int lifetime)
{
    showMessage(message, FlashMessage::Warning, lifetime);
}

void FlashMessageManager::setParent(QWidget *parent)
{
    if(sInstance == nullptr)
        sInstance = new FlashMessageManager(qApp);
    sInstance->mParent = parent;
}

FlashMessageManager::FlashMessageManager(QObject *parent) : QObject(parent)
{
}

void FlashMessageManager::flashMessageClosed(FlashMessage *flashmessage)
{
    mFlashMessages.removeOne(flashmessage);
    rearrangeMessage();
}

void FlashMessageManager::rearrangeMessage()
{
    if(mFlashMessages.isEmpty()) return;
    auto desktop = qApp->desktop();
    const QRect &rect = desktop->screenGeometry();
    int curY = SCREEN_Y_MARGIN;
    for(auto flash : mFlashMessages) {
        const QSize &s = flash->frameSize();
        flash->move(rect.width() - SCREEN_X_MARGIN - s.width(), curY);
        curY += s.height() + Y_SPACE;
    }
}
