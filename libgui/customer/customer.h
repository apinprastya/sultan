/*
 * cusomer.h
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
#ifndef CUSOMER_H
#define CUSOMER_H

#include <QString>
#include <QVariantMap>

namespace LibGUI {

class Customer {
  public:
    int id = 0;
    int reward = 0;
    double credit = 0;
    QString number;
    QString name;
    Customer();
    void fill(const QVariantMap &data);
    inline bool isValid() { return id > 0; }
    void reset();
    QVariantMap toMap();
};

} // namespace LibGUI
#endif // CUSOMER_H
