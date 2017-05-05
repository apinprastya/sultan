/*
 * global_constant
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
#ifndef GLOBAL_CONSTANT_H
#define GLOBAL_CONSTANT_H

#include <QString>

namespace LibG {

namespace CONSTANT {
    const QString APP_NAME          = QStringLiteral("Turbin");
    const QString WINDOW_TITLE      = QStringLiteral("Turbin - %1");
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
}

namespace MSG_COMMAND {
    const int INSERT        = 0;
    const int UPDATE        = 1;
    const int DELETE        = 2;
    const int GET           = 3;
    const int QUERY         = 4;
    //custom command
    const int LOGIN         = 5;
}

}

#endif // GLOBAL_CONSTANT_H
