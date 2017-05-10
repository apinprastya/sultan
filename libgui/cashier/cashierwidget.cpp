#include "cashierwidget.h"
#include "ui_cashierwidget.h"
#include "global_constant.h"
#include "message.h"
#include "db_constant.h"
#include "cashiertablemodel.h"
#include <QMessageBox>
#include <QDebug>

using namespace LibG;
using namespace LibGUI;

CashierWidget::CashierWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CashierWidget),
    mModel(new CashierTableModel(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->tableView->setModel(mModel);
    ui->labelVersion->setText(CONSTANT::ABOUT_APP_NAME.arg(qApp->applicationVersion()));
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(barcodeEntered()));
}

CashierWidget::~CashierWidget()
{
    delete ui;
}

void CashierWidget::showEvent(QShowEvent *event)
{
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    QWidget::showEvent(event);
}

void CashierWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE)) {
        ui->lineBarcode->selectAll();
        ui->lineBarcode->setEnabled(true);
        if(msg->isSuccess()) {
            ui->labelName->setText(msg->data("item").toMap()["name"].toString());
            const QVariantList &list = msg->data("prices").toList();
            double price = list.first().toMap()["price"].toDouble();
            for(int i = 1; i < list.size(); i++) {
                if(list[i].toMap()["count"].toFloat() == 1.0f) {
                    price = list[i].toMap()["price"].toDouble();
                    break;
                }
            }
            ui->labelPrice->setText(QLocale().toString(price));
            //qDebug() << msg->data();
        } else {
            QMessageBox::warning(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void CashierWidget::barcodeEntered()
{
    QString barcode = ui->lineBarcode->text();
    if(barcode.contains("*")) {
        const QStringList ls = barcode.split("*");
        if(ls.size() > 1) {
            barcode = ls[1];
            mCount = ls[0].toFloat();
        }
    }
    LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE);
    msg.addData("barcode", barcode);
    sendMessage(&msg);
}
