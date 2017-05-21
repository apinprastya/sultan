/*
 * print_constant.h
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
#ifndef PRINT_CONSTANT_H
#define PRINT_CONSTANT_H

namespace LibPrint {

namespace BORDER {
    /*const char TOP_LEFT          = 0xDA;
    const char TOP_MIDDLE        = 0xCA;
    const char TOP_RIGHT         = 0xBF;
    const char RIGHT_MIDDLE      = 0xB4;
    const char BOTTOM_RIGHT      = 0xD9;
    const char BOTTOM_MIDDLE     = 0xC1;
    const char BOTTOM_LEFT       = 0xC0;
    const char LEFT_MIDDLE       = 0xC3;
    const char VERTICAL          = 0xB3;
    const char HORIZONTAL        = 0xC4;
    const char MIDDLE            = 0xC5;*/
    const char TOP_LEFT          = '-';
    const char TOP_MIDDLE        = '-';
    const char TOP_RIGHT         = '-';
    const char RIGHT_MIDDLE      = '-';
    const char BOTTOM_RIGHT      = '-';
    const char BOTTOM_MIDDLE     = '-';
    const char BOTTOM_LEFT       = '-';
    const char LEFT_MIDDLE       = '-';
    const char VERTICAL          = '|';
    const char HORIZONTAL        = '-';
    const char MIDDLE            = '-';
}

namespace ESCP {
    const char ESCP                     = 0x1B;
    const char LINE_FEED                = 0xA;
}

}

#endif // PRINT_CONSTANT_H
