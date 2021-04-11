/*
 * useraction.h
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
#ifndef USERACTION_H
#define USERACTION_H

#include "serveraction.h"

namespace LibServer {

class UserAction : public ServerAction {
  public:
    UserAction();
    LibG::Message login(LibG::Message *msg);
    LibG::Message changeMyPassword(LibG::Message *msg);
};

} // namespace LibServer
#endif // USERACTION_H
