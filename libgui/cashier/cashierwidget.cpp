#include "cashierwidget.h"
#include "ui_cashierwidget.h"
#include "global_constant.h"

using namespace LibG;
using namespace LibGUI;

CashierWidget::CashierWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CashierWidget)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelVersion->setText(CONSTANT::ABOUT_APP_NAME.arg(qApp->applicationVersion()));
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
}

CashierWidget::~CashierWidget()
{
    delete ui;
}

void CashierWidget::messageReceived(LibG::Message *msg)
{

}
