/*
 * print_global.h
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
#ifndef PRINT_GLOBAL_H
#define PRINT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef PRINT_LIBRARY_STATIC
#define PRINTSHARED_EXPORT
#else
#if defined(PRINT_LIBRARY)
#define PRINTSHARED_EXPORT Q_DECL_EXPORT
#else
#if defined(SINGLEBIN)
#define PRINTSHARED_EXPORT
#else
#define PRINTSHARED_EXPORT Q_DECL_IMPORT
#endif
#endif
#endif

#endif // PRINT_GLOBAL_H
