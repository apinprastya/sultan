/*
 * cusomer.cpp
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
#include "customer.h"

using namespace LibGUI;

Customer::Customer()
{

}

void Customer::fill(const QVariantMap &data)
{
    id = data["id"].toInt();
    number = data["number"].toString();
    name = data["name"].toString();
    reward = data["reward"].toInt();
    debt = data["debt"].toDouble();
}

void Customer::reset()
{
    id = 0;
    number = QString();
    name = QString();
    reward = 0;
    debt = 0;
}
