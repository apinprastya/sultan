/*
 * gui_global.h
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
#ifndef GUI_GLOBAL_H
#define GUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GUI_LIBRARY_STATIC
#define GUISHARED_EXPORT
#else
#if defined(GUI_LIBRARY)
#define GUISHARED_EXPORT Q_DECL_EXPORT
#else
#if defined(SINGLEBIN)
#define GUISHARED_EXPORT
#else
#define GUISHARED_EXPORT Q_DECL_IMPORT
#endif
#endif
#endif

#endif // GUI_GLOBAL_H
