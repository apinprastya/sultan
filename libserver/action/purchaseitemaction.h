/*
 * purchaseitemaction.h
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
#ifndef PURCHASEITEMACTION_H
#define PURCHASEITEMACTION_H

#include "serveraction.h"

namespace LibServer {

class PurchaseItemAction : public ServerAction {
  public:
    PurchaseItemAction();
    LibG::Message insert(LibG::Message *msg) override;
    LibG::Message update(LibG::Message *msg) override;
    LibG::Message del(LibG::Message *msg) override;
    LibG::Message summary(LibG::Message *msg);

  protected:
    QMap<QString, QString> fieldMap() const override;
    void selectAndJoin() override;

  private:
    void updatePurchaseTotal(int purchaseid);
};

} // namespace LibServer
#endif // PURCHASEITEMACTION_H
