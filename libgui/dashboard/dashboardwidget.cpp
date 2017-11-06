#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include "global_constant.h"
#include "tilewidget.h"
#include "message.h"
#include "util.h"
#include "preference.h"

using namespace LibGUI;
using namespace LibG;

DashboardWidget::DashboardWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardWidget),
    mStockValue(new TileWidget(tr("Stock value"), this)),
    mDebt(new TileWidget(tr("Debt"), this)),
    mCredit(new TileWidget(tr("Credit"), this)),
    mSales(new TileWidget(tr("Sales"), this)),
    mMargin(new TileWidget(tr("Margin"), this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->tileLayout->addWidget(mStockValue);
    ui->tileLayout->addWidget(mDebt);
    ui->tileLayout->addWidget(mCredit);
    ui->tileLayout->addWidget(mSales);
    ui->tileLayout->addWidget(mMargin);
    ui->tileLayout->addStretch();
    Message msg(MSG_TYPE::DASHBOARD, MSG_COMMAND::DASH_TILE);
    msg.addData("start", Util::getBeginningOfMonth());
    msg.addData("end", QDate::currentDate());
    sendMessage(&msg);
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}

void DashboardWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::DASHBOARD, MSG_COMMAND::DASH_TILE)) {
        mStockValue->setValue(Preference::formatMoney(msg->data("stock_value").toDouble()));
        mDebt->setValue(Preference::formatMoney(msg->data("debt").toDouble()));
        mCredit->setValue(Preference::formatMoney(msg->data("credit").toDouble()));
        mSales->setValue(Preference::formatMoney(msg->data("sales").toDouble()));
        mMargin->setValue(Preference::formatMoney(msg->data("margin").toDouble()));
    }
}
