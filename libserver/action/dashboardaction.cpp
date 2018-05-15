#include "dashboardaction.h"
#include "global_constant.h"
#include "db.h"
#include "queryhelper.h"
#include <QDate>

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

DashboardAction::DashboardAction():
    ServerAction("", "")
{
    mFunctionMap.insert(MSG_COMMAND::DASH_TILE, std::bind(&DashboardAction::tile, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::DASH_SALES, std::bind(&DashboardAction::sales, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::DASH_STOCK, std::bind(&DashboardAction::stock, this, std::placeholders::_1));
    mFunctionMap.insert(MSG_COMMAND::DASH_DEBT, std::bind(&DashboardAction::debt, this, std::placeholders::_1));
}

Message DashboardAction::tile(Message *msg)
{
    LibG::Message message(msg);
    const QString &start = msg->data("start").toString();
    const QString &end = msg->data("end").toString();
    QVariantMap data;
    //stock value
    DbResult res = mDb->select("sum(stock * buy_price) as total")->
            where("stock > 0")->
            where(QString("(flag & %1) = 0").arg(ITEM_FLAG::PACKAGE))->
            where(QString("(flag & %1) = 0").arg(ITEM_FLAG::HAS_INGRIDIENT))->get("items");
    data["stock_value"] = res.first()["total"];
    //debt : 1. suplier, 2. sold retur
    double debt = 0;
    res = mDb->select("COALESCE(sum(final), 0) as total")->where("status = ", PAYMENT_STATUS::UNPAID)->
            where("payment_type = ", PURCHASEPAYMENT::TEMPO)->get("purchases");
    debt += res.first()["total"].toDouble();
    res = mDb->select("COALESCE(sum(total), 0) as total")->where("status = ", PURCHASE_RETURN_STATUS::UNRETURN)->get("solditemreturns");
    debt += res.first()["total"].toDouble();
    data["debt"] = debt;
    //credit : 1. customer credit, 2. purchase return
    double credit = 0;
    res = mDb->select("COALESCE(sum(credit), 0) as total")->get("customercredits");
    debt += res.first()["total"].toDouble();
    res = mDb->select("COALESCE(sum(total), 0) as total")->where("status = ", PURCHASE_RETURN_STATUS::UNRETURN)->get("purchaseitemreturns");
    debt += res.first()["total"].toDouble();
    data["credit"] = credit;
    //sales 1 month
    res = mDb->select("COALESCE(sum(final), 0) as total")->where("DATE(created_at) >= ", start)->
            where("DATE(created_at) <= ", end)->where(QString("(flag & %1) = 0").arg(ITEM_FLAG::ITEM_LINK))->get("solditems");
    data["sales"] = res.first()["total"];
    //margin 1 month
    res = mDb->select("COALESCE(sum(final - buy_price), 0) as total")->where("DATE(created_at) >= ", start)->
            where("DATE(created_at) <= ", end)->where(QString("(flag & %1) = 0").arg(ITEM_FLAG::ITEM_LINK))->get("solditems");
    data["sales"] = res.first()["total"];
    message.setData(data);
    return message;
}

Message DashboardAction::sales(Message *msg)
{
    LibG::Message message(msg);
    return message;
}

Message DashboardAction::stock(Message *msg)
{
    LibG::Message message(msg);
    return message;
}

Message DashboardAction::debt(Message *msg)
{
    LibG::Message message(msg);
    return message;
}
