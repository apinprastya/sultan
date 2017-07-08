/*
 * purchaseaction.h
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
#ifndef PURCHASEACTION_H
#define PURCHASEACTION_H

#include "serveraction.h"

namespace LibServer {

class PurchaseAction: public ServerAction
{
public:
    PurchaseAction();
    LibG::Message del(LibG::Message *msg) override;
    LibG::Message summary(LibG::Message *msg);

protected:
    void afterInsert(const QVariantMap &data) override;
    void afterUpdate(const QVariantMap &data) override;
    void selectAndJoin() override;
    QMap<QString, QString> fieldMap() const override;
    void insertTransaction(const QVariantMap &data);
    void updateTransaction(const QVariantMap &data);
};

}
#endif // PURCHASEACTION_H
