/*
 * global_setting_const
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
#ifndef GLOBAL_SETTING_CONST_H
#define GLOBAL_SETTING_CONST_H

#include <QString>

namespace LibG {

namespace SETTING {
    const QString SETTING_OK                    = QStringLiteral("global/ok");
    const QString APP_TYPE                      = QStringLiteral("global/app_type");
    const QString DATABASE                      = QStringLiteral("global/database");
    const QString MYSQL_HOST                    = QStringLiteral("global/mysql_host");
    const QString MYSQL_USERNAME                = QStringLiteral("global/mysql_username");
    const QString MYSQL_PASSWORD                = QStringLiteral("global/mysql_password");
    const QString MYSQL_PORT                    = QStringLiteral("global/mysql_port");
    const QString MYSQL_DB                      = QStringLiteral("global/mysql_dbname");
    const QString MYSQL_OK                      = QStringLiteral("global/mysql_ok");
    const QString APP_PORT                      = QStringLiteral("global/app_port");
    const QString SERVER_ADDRESS                = QStringLiteral("global/server_address");
    const QString SERVER_PORT                   = QStringLiteral("global/server_port");

    const QString MARKET_NAME                   = QStringLiteral("application/market_name");
    const QString MARKET_SUBNAME                = QStringLiteral("application/market_subname");
    const QString MACHINE_ID                    = QStringLiteral("application/machine_id");
    const QString MACHINE_NAME                  = QStringLiteral("application/machine_name");

    const QString APPLICATION_LANGUAGE          = QStringLiteral("application/language");
    const QString CASHIER_NAMEBASED             = QStringLiteral("application/name_based");
    const QString LOCALE_LANGUAGE               = QStringLiteral("application/locale_language");
    const QString LOCALE_COUNTRY                = QStringLiteral("application/locale_country");
    const QString LOCALE_USE_SIGN               = QStringLiteral("application/locale_use_sign");
    const QString LOCALE_SIGN                   = QStringLiteral("application/locale_sign");
    const QString LOCALE_DECIMAL                = QStringLiteral("application/locale_decimal");

    const QString USE_TAX                       = QStringLiteral("application/use_tax");
    const QString TAX_VALUE                     = QStringLiteral("application/tax_value");

    const QString PRINTER_CASHIER_TYPE          = QStringLiteral("printer/cashier_type");
    const QString PRINTER_CASHIER_NAME          = QStringLiteral("printer/cashier_name");
    const QString PRINTER_CASHIER_DEVICE        = QStringLiteral("printer/cashier_device");
    const QString PRINTER_CASHIER_TITLE         = QStringLiteral("printer/cashier_title");
    const QString PRINTER_CASHIER_SUBTITLE      = QStringLiteral("printer/cashier_subtitle");
    const QString PRINTER_CASHIER_FOOTER        = QStringLiteral("printer/cashier_footer");
    const QString PRINTER_CASHIER_CPI10         = QStringLiteral("printer/cashier_cpi10");
    const QString PRINTER_CASHIER_CPI12         = QStringLiteral("printer/cashier_cpi12");
    const QString PRINTER_CASHIER_AUTOCUT       = QStringLiteral("printer/cashier_autocut");
    const QString PRINTER_CASHIER_KICK          = QStringLiteral("printer/cashier_kick");
    const QString PRINTER_CASHIER_LINEFEED      = QStringLiteral("printer/cashier_linefeed");

    const QString PRINTER_CASHIER_PRICE_LINEFEED      = QStringLiteral("printer/cashier_price_linefeed");

    const QString NUMBER_DATE                   = QStringLiteral("number/date");
    const QString NUMBER_VALUE                  = QStringLiteral("number/value");
}

}

#endif // GLOBAL_SETTING_CONST_H
