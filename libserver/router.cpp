/*
 * router.cpp
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
#include "router.h"
#include "message.h"
#include "serveraction.h"
#include "global_constant.h"
#include "action/useraction.h"
#include "action/suplieraction.h"
#include "action/itemaction.h"
#include "action/sellpriceactions.h"
#include "action/categoryaction.h"
#include "action/purchaseaction.h"
#include "action/purchaseitemaction.h"
#include "action/soldaction.h"
#include "action/solditemaction.h"
#include "action/customeraction.h"
#include "action/customerpointaction.h"
#include "action/cusomercreditaction.h"
#include "action/machineaction.h"
#include "action/rewardaction.h"
#include "action/rewardpoinaction.h"
#include "action/bankaction.h"
#include "action/transactionaction.h"
#include "action/purchasereturnaction.h"
#include "db.h"
#include "queryhelper.h"
#include <QStringBuilder>
#include <QDebug>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

Router::Router()
{
    //define the user defined filter here
    LibDB::QueryHelper::installUserDefinedFilter(FILTER::CATEGORY_IN, [](Db *db, const QString &key, int type, const QVariantMap &data) {
       if(type == FILTER::CATEGORY_IN) {
            db->where(key % " IN (SELECT child_id FROM categorychilds WHERE category_id = " % data["value"].toString() % ")");
       }
    });
}

LibG::Message Router::handler(LibG::Message msg)
{
    QScopedPointer<ServerAction> action(getServerAction(msg.type()));
    if(action != nullptr)
        return action->exec(&msg);
    msg.setStatus(STATUS::ERROR);
    return msg;
}

ServerAction *Router::getServerAction(int type)
{
    switch(type) {
    case MSG_TYPE::USER:
        return new UserAction();
    case MSG_TYPE::SUPLIER:
        return new SuplierAction();
    case MSG_TYPE::ITEM:
        return new ItemAction();
    case MSG_TYPE::SELLPRICE:
        return new SellPriceActions();
    case MSG_TYPE::CATEGORY:
        return new CategoryAction();
    case MSG_TYPE::PURCHASE:
        return new PurchaseAction();
    case MSG_TYPE::PURCHASE_ITEM:
        return new PurchaseItemAction();
    case MSG_TYPE::SOLD:
        return new SoldAction();
    case MSG_TYPE::SOLD_ITEM:
        return new SoldItemAction();
    case MSG_TYPE::CUSTOMER:
        return new CustomerAction();
    case MSG_TYPE::CUSTOMER_POINT:
        return new CustomerPointAction();
    case MSG_TYPE::CUSTOMER_CREDIT:
        return new CusomerCreditAction();
    case MSG_TYPE::MACHINE:
        return new MachineAction();
    case MSG_TYPE::REWARD:
        return new RewardAction();
    case MSG_TYPE::REWARD_POIN:
        return new RewardPoinAction();
    case MSG_TYPE::BANK:
        return new BankAction();
    case MSG_TYPE::TRANSACTION:
        return new TransactionAction();
    case MSG_TYPE::PURCHASE_RETURN:
        return new PurchaseReturnAction();
    }
    return nullptr;
}

