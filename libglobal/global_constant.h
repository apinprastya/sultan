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
}

namespace PERMISSION {
    const int ADMINISTRATOR     = 9876;
    const int USER              = 1;
    const int SUPLIER           = 2;
    const int ITEM_RW           = 3;
    const int CASHIER           = 4;
    const int CATEGORY          = 5;
    const int PURCASHE          = 6;
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
    const int INTERNAL      = 0;
    const int USER          = 1;
    const int SUPLIER       = 2;
    const int ITEM          = 3;
    const int SELLPRICE     = 4;
    const int CATEGORY      = 5;
    const int PURCHASE      = 6;
    const int PURCHASE_ITEM = 7;
    const int SOLD          = 8;
    const int SOLD_ITEM     = 9;
}

namespace MSG_COMMAND {
    const int INSERT        = 0;
    const int UPDATE        = 1;
    const int DEL           = 2;
    const int GET           = 3;
    const int QUERY         = 4;
    //custom command
    const int LOGIN         = 5;
    const int CASHIER_PRICE = 6;
    const int NEW_SOLD      = 7;
    const int SOLD_SUMMARY  = 8;
    const int SOLD_ITEM_REPORT  = 9;
}

namespace PRINT_TYPE {
    const int DEVICE    = 0;
    const int SPOOL     = 1;
}

}

#endif // GLOBAL_CONSTANT_H
