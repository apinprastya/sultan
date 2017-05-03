/*
 * global_setting_const
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
#ifndef GLOBAL_SETTING_CONST_H
#define GLOBAL_SETTING_CONST_H

#include <QString>

namespace LibG {

namespace SETTING {
    const QString SETTING_OK                    = QStringLiteral("global/ok");
    const QString APP_TYPE                      = QStringLiteral("global/app_type");
    const QString MYSQL_HOST                    = QStringLiteral("global/mysql_host");
    const QString MYSQL_USERNAME                = QStringLiteral("global/mysql_username");
    const QString MYSQL_PASSWORD                = QStringLiteral("global/mysql_password");
    const QString MYSQL_PORT                    = QStringLiteral("global/mysql_port");
    const QString MYSQL_DB                      = QStringLiteral("global/mysql_dbname");
}

}

#endif // GLOBAL_SETTING_CONST_H
