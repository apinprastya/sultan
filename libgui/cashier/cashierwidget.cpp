#include "cashierwidget.h"
#include "ui_cashierwidget.h"
#include "global_constant.h"
#include "message.h"
#include "db_constant.h"
#include "cashiertablemodel.h"
#include "cashieritem.h"
#include "guiutil.h"
#include "keyevent.h"
#include "paycashdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QKeyEvent>
#include <QShortcut>
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
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    GuiUtil::setColumnWidth(ui->tableView, QList<int>() << 50 << 150 << 50 << 120 << 120);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->labelVersion->setText(CONSTANT::ABOUT_APP_NAME.arg(qApp->applicationVersion()));
    auto keyevent = new KeyEvent(ui->tableView);
    keyevent->addConsumeKey(Qt::Key_Return);
    keyevent->addConsumeKey(Qt::Key_Delete);
    ui->tableView->installEventFilter(keyevent);
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(barcodeEntered()));
    connect(mModel, SIGNAL(totalChanged(double)), SLOT(totalChanged(double)));
    connect(mModel, SIGNAL(selectRow(QModelIndex)), SLOT(selectRow(QModelIndex)));
    connect(keyevent, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(tableKeyPressed(QObject*,QKeyEvent*)));
    new QShortcut(QKeySequence(Qt::Key_F4), this, SLOT(payCash()));
    new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(payCashless()));
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
            const QString &name = msg->data("item").toMap()["name"].toString();
            const QString &barcode = msg->data("item").toMap()["barcode"].toString();
            ui->labelName->setText(name);
            const QVariantList &list = msg->data("prices").toList();
            double price = list.first().toMap()["price"].toDouble();
            for(int i = 1; i < list.size(); i++) {
                if(list[i].toMap()["count"].toFloat() == 1.0f) {
                    price = list[i].toMap()["price"].toDouble();
                    break;
                }
            }
            ui->labelPrice->setText(QLocale().toString(price));
            mModel->addItem(mCount, name, barcode, list);
        } else {
            QMessageBox::warning(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void CashierWidget::barcodeEntered()
{
    QString barcode = ui->lineBarcode->text();
    if(barcode.isEmpty()) return;
    if(barcode.contains("*")) {
        const QStringList ls = barcode.split("*");
        if(ls.size() > 1) {
            barcode = ls[1];
            mCount = ls[0].toFloat();
        }
    } else {
        mCount = 1.0f;
    }
    LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE);
    msg.addData("barcode", barcode);
    sendMessage(&msg);
}

void CashierWidget::totalChanged(double value)
{
    ui->labelTotal->setText(QLocale().toString(value));
}

void CashierWidget::selectRow(const QModelIndex &index)
{
    ui->tableView->selectRow(index.row());
    ui->tableView->scrollTo(index);
}

void CashierWidget::tableKeyPressed(QObject */*sender*/, QKeyEvent *event)
{
    const QModelIndex &index = ui->tableView->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<CashierItem*>(index.internalPointer());
    if(event->key() == Qt::Key_Return) {
        bool ok = false;
        double count = QInputDialog::getDouble(this, tr("Edit count"), tr("Input new count"), item->count, 0, 1000000, 1, &ok);
        if(ok)
            mModel->addItem(count - item->count, item->name, item->barcode);
    } else if(event->key() == Qt::Key_Delete){
        mModel->addItem(-item->count, item->name, item->barcode);
    }
}

void CashierWidget::payCash()
{
    if(mModel->isEmpty()) return;
    PayCashDialog dialog(mModel->getTotal(), this);
    dialog.exec();
    if(dialog.isPayed()) {
        //send the items to server
    }
}

void CashierWidget::payCashless()
{
}

