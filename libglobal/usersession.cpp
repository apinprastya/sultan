/*
 * usersession.cpp
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
#include "usersession.h"
#include "global_constant.h"
#include <QDebug>

using namespace LibG;

static UserSession *sInstance = nullptr;

UserSession::UserSession() {}

LibG::UserSession *LibG::UserSession::init(const QVariantMap &data) {
    if (sInstance == nullptr)
        sInstance = new UserSession();
    sInstance->mName = data["name"].toString();
    sInstance->mUsername = data["username"].toString();
    sInstance->mId = data["id"].toInt();
    sInstance->mPermission.clear();
    QStringList per = data["permission"].toString().split(",");
    for (const QString &str : per) {
        const QString &s = str.trimmed();
        if (s.isEmpty())
            continue;
        int val = s.toInt();
        sInstance->mPermission.append(val);
    }
    return sInstance;
}

QString LibG::UserSession::username() { return sInstance->mUsername; }

QString LibG::UserSession::name() { return sInstance->mName; }

int UserSession::id() { return sInstance->mId; }

bool LibG::UserSession::hasPermission(int permission) {
    if (sInstance->mPermission.contains(PERMISSION::ADMINISTRATOR))
        return true;
    return sInstance->mPermission.contains(permission);
}

void UserSession::destroy() {
    if (sInstance)
        delete sInstance;
}

void UserSession::logout() { sInstance->mId = 0; }

bool UserSession::isLoggedIn() { return sInstance->mId > 0; }
