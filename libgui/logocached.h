/*
 * logocached.h
 * Copyright 2023, Apin <apin.klas@gmail.com>
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

#ifndef LOGOCACHED_H
#define LOGOCACHED_H

#include "gui_global.h"

#include <QObject>
#include <QPixmap>

namespace LibGUI {

class GUISHARED_EXPORT LogoCached : public QObject {
    Q_OBJECT
  public:
    static void createInstance(QObject *parent = nullptr);
    static QPixmap logo32();
    static QPixmap logo64();
    static QPixmap logo128();
    static QPixmap logo256();
    static QPixmap logo512();

  private:
    static LogoCached *instance;
    QPixmap mLogo32;
    QPixmap mLogo64;
    QPixmap mLogo128;
    QPixmap mLogo256;
    QPixmap mLogo512;

    LogoCached(QObject *parent = nullptr);
    void load();
    QPixmap getMainPixmap();
};

} // namespace LibGUI

#endif // LOGOCACHED_H
