/*
 * usersession.h
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
#ifndef USERSESSION_H
#define USERSESSION_H

#include "global_global.h"
#include <QVariantMap>

namespace LibG {

class GLOBALSHARED_EXPORT UserSession {
  public:
    static UserSession *init(const QVariantMap &data);
    static QString username();
    static QString name();
    static int id();
    static bool hasPermission(int permission);
    static void destroy();
    static void logout();
    static bool isLoggedIn();

  private:
    UserSession();

    int mId = 0;
    QString mUsername;
    QString mName;
    QList<int> mPermission;
};

} // namespace LibG
#endif // USERSESSION_H
