/*
 * db_constant.h
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

#ifndef DB_CONSTANT
#define DB_CONSTANT

namespace COMPARE {
    const int EQUAL             = 0;
    const int NEQUAL            = 1;
    const int LESS              = 2;
    const int GREATER           = 3;
    const int LESS_EQUAL        = 4;
    const int GREATER_EQUAL     = 5;
    const int ISNULL            = 6;
    const int ISNOTNULL         = 7;
    const int LIKE              = 8;
    const int IGNORE            = 10;
    const int LIKE_NATIVE       = 11;
    const int FLAG_ENABLE       = 12;
    const int FLAG_DISABLE      = 13;
    const int FLAG_ALL          = 14;
    const int USER_DEFINE       = 100;
}

#endif // DB_CONSTANT

