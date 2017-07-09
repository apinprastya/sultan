/*
 * global_constant
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
#ifndef GLOBAL_CONSTANT_H
#define GLOBAL_CONSTANT_H

#include <QString>

namespace LibG {

namespace CONFIG {
    const int ITEMS_PER_LOAD        = 50;
}

namespace CONSTANT {
    const QString APP_NAME          = QStringLiteral("Sultan");
    const QString WINDOW_TITLE      = QStringLiteral("%1 - Sultan");
    const QString ABOUT_APP_NAME    = QStringLiteral("Sultan %1");
    const QString URL_UPDATE        = QStringLiteral("https://sultanupdate.lekapin.com/check?arc=%1&qt=%2");
    const QString URL_DOWNLOAD      = QStringLiteral("https://sultanupdate.lekapin.com/assets/upload/sultan_%1_%2_%3");
}

namespace PERMISSION {
    const int ADMINISTRATOR     = 9876;
    const int USER              = 1;
    const int SUPLIER           = 2;
    const int ITEM_RW           = 3;
    const int CASHIER           = 4;
    const int CATEGORY          = 5;
    const int PURCASHE          = 6;
    const int REPORT            = 7;
    const int CUSTOMER          = 8;
    const int BANK              = 9;
    const int REPORT_MONEY      = 10;
    const int REPORT_TRANS      = 11;
}

namespace APPLICATION_TYPE {
    const int SERVER    = 0;
    const int CLIENT    = 1;
}

namespace STATUS {
    const int OK = 0;
    const int ERROR = 1;
}

namespace MSG_TYPE {
    const int INTERNAL          = 0;
    const int USER              = 1;
    const int SUPLIER           = 2;
    const int ITEM              = 3;
    const int SELLPRICE         = 4;
    const int CATEGORY          = 5;
    const int PURCHASE          = 6;
    const int PURCHASE_ITEM     = 7;
    const int SOLD              = 8;
    const int SOLD_ITEM         = 9;
    const int CUSTOMER          = 10;
    const int CUSTOMER_POINT    = 11;
    const int CUSTOMER_CREDIT   = 12;
    const int MACHINE           = 13;
    const int REWARD            = 14;
    const int REWARD_POIN       = 15;
    const int BANK              = 16;
    const int TRANSACTION       = 17;
    const int PURCHASE_RETURN   = 18;
}

namespace MSG_COMMAND {
    const int INSERT                    = 0;
    const int UPDATE                    = 1;
    const int DEL                       = 2;
    const int GET                       = 3;
    const int QUERY                     = 4;
    //custom command
    const int LOGIN                     = 5;
    const int CASHIER_PRICE             = 6;
    const int NEW_SOLD                  = 7;
    const int SOLD_SUMMARY              = 8;
    const int SOLD_ITEM_REPORT          = 9;
    const int EXPORT                    = 10;
    const int IMPORT                    = 11;
    const int CHANGE_MY_PASSWORD        = 12;
    const int SUMMARY                   = 13;
    const int SUMMARY_TRANSACTION       = 14;
    const int SUMMARY_MONEY             = 15;
}

namespace PRINT_TYPE {
    const int DEVICE    = 0;
    const int SPOOL     = 1;
}

namespace FILTER {
    const int CATEGORY_IN = 100; //this value taken from DB COMPARE USER_DEFINED
}

namespace PAYMENT {
    const int CASH      = 0;
    const int NON_CASH  = 1;
}

namespace PURCHASEPAYMENT {
    const int DIRECT   = 0;
    const int TEMPO    = 1;
}

namespace PAYMENT_STATUS {
    const int UNPAID = 0;
    const int PAID   = 1;
}

namespace POIN_TYPE {
    const int EXCHANGE  = 0;
    const int MANUAL    = 1;
}

namespace CARD_TYPE {
    const int DEBIT     = 0;
    const int CREDIT    = 1;
}

namespace TRANSACTION_TYPE {
    const int INCOME        = 0;
    const int EXPENSE       = 1;
}

namespace TRANSACTION_LINK_TYPE {
    const int TRANSACTION       = 0;
    const int SOLD              = 1;
    const int PURCHASE          = 2;
    const int CUSTOMER_CREDIT   = 16;
}

namespace MONEY_TYPE {
    const int INCOME        = 0;
    const int EXPENSE       = 1;
}

namespace MONEY_LINK_TYPE {
    const int SOLD              = 0;
    const int PURCHASE          = 1;
    const int TRANSACTION       = 2;
    const int CUSTOMER_CREDIT   = 3;
}

namespace PURCHASE_RETURN_TYPE {
    const int ITEM      = 0;
    const int MONEY     = 1;
}

namespace PURCHASE_RETURN_STATUS {
    const int UNRETURN      = 0;
    const int RETURNED      = 1;
}

}

#endif // GLOBAL_CONSTANT_H
