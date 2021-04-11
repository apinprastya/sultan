/*
 * preference.h
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
#ifndef PREFERENCE_H
#define PREFERENCE_H

#include "global_global.h"
#include <QSettings>
#include <QString>

namespace LibG {

class GLOBALSHARED_EXPORT Preference {
  public:
    Preference();
    ~Preference();
    static void destroy();
    static void createInstance();
    static void setValue(const QString &key, const QVariant &data);
    static int getInt(const QString &key, int def = 0);
    static QString getString(const QString &key, const QString &def = QString());
    static bool getBool(const QString &key, bool def = false);
    static double getDouble(const QString &key, double def = 0.0f);
    static void sync();

    static void applyApplicationSetting();
    static QString formatMoney(double val);
    static QString formatMoney(int val);
    static QString formatMoney(float val);
    static QString formatFloat(float val);

  private:
    static Preference *instance();
    QSettings *mSetting;
};

} // namespace LibG

#endif // PREFERENCE_H
