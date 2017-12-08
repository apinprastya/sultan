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
    const QString BARCODE_CLOUD     = QStringLiteral("https://barcode.indoarm.com/?barcode=%1");
}

namespace GOOGLE {
    const QString CLIENT_ID         = QStringLiteral("1016493816749-tivbcpabpqpf9lpa5er1f7t8rd8vls93.apps.googleusercontent.com");
    const QString CLIENT_SECRET     = QStringLiteral("6tAzo9zJp_pTwSD_L6-KBOqc");
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
    const int CHECK_STOCK       = 12;
    const int INITIAL_STOCK     = 13;
    const int UNIT              = 14;
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
    const int DATABASE          = 19;
    const int CHECKSTOCK        = 20;
    const int UNIT              = 21;
    const int STOCKCARD         = 22;
    const int ITEMLINK          = 23;
    const int SOLDRETURN        = 24;
    const int CONFIG            = 25;
    const int BROADCAST         = 255;
}

namespace MSG_COMMAND {
    const int INSERT                    = 0;
    const int UPDATE                    = 1;
    const int DEL                       = 2;
    const int GET                       = 3;
    const int QUERY                     = 4;
    const int RESTORE_DELETED           = 5;
    //custom command
    const int LOGIN                     = 20;
    const int CASHIER_PRICE             = 21;
    const int NEW_SOLD                  = 22;
    const int SOLD_SUMMARY              = 23;
    const int SOLD_ITEM_REPORT          = 24;
    const int EXPORT                    = 25;
    const int IMPORT                    = 26;
    const int CHANGE_MY_PASSWORD        = 27;
    const int SUMMARY                   = 28;
    const int SUMMARY_TRANSACTION       = 29;
    const int SUMMARY_MONEY             = 30;
    const int RESET                     = 31;
    const int CONFIG_INSERT_UPDATE      = 32;
}

namespace MSG_BROADCAST {
    const int SETTING_CHANGES           = 0;
}

namespace GUI_MESSAGE {
    const int MSG_CONNECTION_SUCCESS = 0;
    const int MSG_CONNECTION_FAILED  = 1;
    const int MSG_CONNECTION_TIMEOUT = 2;
}

namespace PRINT_TYPE {
    const int DEVICE    = 0;
    const int SPOOL     = 1;
    const int USB       = 2;
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
    const int BUY_RETURN        = 17;
    const int SOLD_RETURN       = 18;
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

namespace CHECKSTOCK_FLAG {
    const int CHECKSTOCK        = 0x1;
    const int INITIAL           = 0x2;
}

namespace ITEM_FLAG {
    const int MULTIPRICE        = 0x1;
    const int EDITABLE_PRICE    = 0x2;
    const int REQUIRE_NOTE      = 0x4;
    const int CALCULATE_STOCK   = 0x8;
    const int PURCHASE          = 0x10;
    const int HAS_INGRIDIENT    = 0x20;
    const int SELLABLE          = 0x40;
    const int PROCESS_FIRST     = 0x80;
    const int PACKAGE           = 0x100;
    const int SERVICE           = 0x200;
    //additional flags start from 1 << 16
    const int ITEM_LINK         = (1 << 16);
}

namespace STOCK_CARD_TYPE {
    const int INITIAL_STOCK     = 0;
    const int PURCHASE          = 1;
    const int SOLD              = 2;
    const int CHECKSTOCK        = 3;
    const int PURCHASE_RETURN   = 4;
    const int SOLD_RETURN       = 5;
}

namespace ITEM_LINK_TYPE {
    const int BOX           = 0;
    const int INGRIDIENT    = 1;
}

namespace CONFIG_DB {
    const int AUTOBARCODE_COUNTER       = 1;
    const int AUTOBARCODE_DIGIT         = 2;
    const int AUTOBARCODE_PREFIX        = 3;
}

}

#endif // GLOBAL_CONSTANT_H
