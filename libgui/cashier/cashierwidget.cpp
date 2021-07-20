/*
 * cashierwidget.cpp
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
#include "cashierwidget.h"
#include "additemunavailabledialog.h"
#include "advancepaymentdialog.h"
#include "cashiercustomerdisplay.h"
#include "cashierhelpdialog.h"
#include "cashieritem.h"
#include "cashierreportdialog.h"
#include "cashiertablemodel.h"
#include "checkpricedialog.h"
#include "customercreditpaymentdialog.h"
#include "db_constant.h"
#include "dbutil.h"
#include "doublespinboxdelegate.h"
#include "editpricecountdialog.h"
#include "escp.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "guiutil.h"
#include "keyevent.h"
#include "message.h"
#include "paycashdialog.h"
#include "paycashlessdialog.h"
#include "paymentcashsuccessdialog.h"
#include "preference.h"
#include "saveloadslotdialog.h"
#include "searchcustomerdialog.h"
#include "searchitemdialog.h"
#include "transaction/addtransactiondialog.h"
#include "transactionlistdialog.h"
#include "ui_cashierwidget.h"
#include "usersession.h"
#include "util.h"
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMessageBox>
#include <QShortcut>
#include <QStringBuilder>
#include <QTimer>
#include <functional>

using namespace LibG;
using namespace LibGUI;
using namespace LibPrint;

CashierWidget::CashierWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::CashierWidget), mModel(new CashierTableModel(bus, this)),
      mPayCashDialog(new PayCashDialog(this)), mAdvancePaymentDialog(new AdvancePaymentDialog(bus, this)),
      mPayCashlessDialog(new PayCashlessDialog(bus, this)), mAddItemDialog(new AddItemUnavailableDialog(bus, this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
    ui->tableView->setModel(mModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    GuiUtil::setColumnWidth(ui->tableView, QList<int>() << 50 << 160 << 150 << 60 << 75 << 90 << 80 << 100);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->labelVersion->setText(CONSTANT::ABOUT_APP_NAME.arg(qApp->applicationVersion()));
    auto keyevent = new KeyEvent(ui->tableView);
    keyevent->addConsumeKey(Qt::Key_Return);
    keyevent->addConsumeKey(Qt::Key_Delete);
    ui->tableView->installEventFilter(keyevent);
    auto barcodeEvent = new KeyEvent(ui->lineBarcode);
    barcodeEvent->setModifier(Qt::ControlModifier);
    barcodeEvent->addConsumeKey(Qt::Key_Return);
    barcodeEvent->addConsumeKey(Qt::Key_Enter);
    ui->lineBarcode->installEventFilter(barcodeEvent);
    connect(ui->lineBarcode, SIGNAL(returnPressed()), SLOT(barcodeEntered()));
    connect(barcodeEvent, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(barcodeWithCtrlPressed()));
    connect(mModel, SIGNAL(totalChanged(double)), SLOT(totalChanged(double)));
    connect(mModel, SIGNAL(selectRow(QModelIndex)), SLOT(selectRow(QModelIndex)));
    connect(keyevent, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(tableKeyPressed(QObject *, QKeyEvent *)));
    connect(mPayCashDialog, SIGNAL(requestPay(int, double, int)), SLOT(payRequested(int, double, int)));
    connect(mAdvancePaymentDialog, SIGNAL(payRequested(int, double, int)), SLOT(payRequested(int, double, int)));
    connect(mPayCashlessDialog, SIGNAL(requestPay(int, double, int)), SLOT(payRequested(int, double, int)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(updateCurrentItem()));
    connect(ui->pushPay, SIGNAL(clicked(bool)), SLOT(payCash()));
    connect(mModel, SIGNAL(requestEdit(QModelIndex, QVariant)), SLOT(editRequest(QModelIndex, QVariant)));
    new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(openHelp()));
    new QShortcut(QKeySequence(Qt::Key_F2), this, SLOT(openSearch()));
    new QShortcut(QKeySequence(Qt::Key_F3), this, SLOT(scanCustomer()));
    new QShortcut(QKeySequence(Qt::Key_F4), this, SLOT(payCash()));
    new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(openDrawer()));
    new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(openPreviousTransaction()));
    new QShortcut(QKeySequence(Qt::Key_F7), this, SLOT(openCheckPrice()));
    new QShortcut(QKeySequence(Qt::Key_F8), this, SLOT(payAdvance()));
    new QShortcut(QKeySequence(Qt::Key_F9), this, SLOT(payCashless()));
    new QShortcut(QKeySequence(Qt::Key_F10), this, SLOT(addNonStockTransaction()));
    new QShortcut(QKeySequence(Qt::Key_F12), this, SLOT(openReport()));
    new QShortcut(QKeySequence(Qt::Key_PageDown), this, SLOT(updateCurrentItem()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete), this, SLOT(newTransaction()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SLOT(newTransaction()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F3), this, SLOT(resetCustomer()));
    new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F3), this, SLOT(openSearchCustomer()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, SLOT(openCustomerCreditPayment()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this, SLOT(focusTable()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this, SLOT(focusBarcode()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this, SLOT(addNewItemNoBarcode()));
    ui->labelTitle->setText(Preference::getString(SETTING::MARKET_NAME, "Sultan Minimarket"));
    ui->labelSubtitle->setText(
        GuiUtil::toHtml(Preference::getString(SETTING::MARKET_SUBNAME, "Jln. Bantul\nYogyakarta")));
    connect(mAddItemDialog, SIGNAL(addNewItem(QVariantMap)), SLOT(addNewItem(QVariantMap)));
    auto inlineEdit = Preference::getBool(SETTING::INLINE_EDIT_QTY);
    if (inlineEdit) {
        connect(ui->tableView, SIGNAL(clicked(QModelIndex)), SLOT(tableClicked(QModelIndex)));
        auto delegate = new DoubleSpinBoxDelegate(ui->tableView);
        ui->tableView->setItemDelegateForColumn(3, delegate);
        mModel->setEnableInlineEdit(true);
    }
}

CashierWidget::~CashierWidget() { delete ui; }

void CashierWidget::showEvent(QShowEvent *event) {
    ui->lineBarcode->setFocus(Qt::TabFocusReason);
    bool isTax = Preference::getBool(SETTING::USE_TAX);
    if (!isTax)
        ui->widgetTax->hide();
    else
        ui->widgetTax->show();
    CashierCustomerDisplay::instance()->setCashierModel(mModel);
    QWidget::showEvent(event);
}

bool CashierWidget::requestClose() {
    if (!mModel->isEmpty()) {
        int ret =
            QMessageBox::question(this, tr("Close confirmation"), tr("Your cart is not empty. Are you sure to close?"));
        return ret == QMessageBox::Yes;
    }
    return true;
}

void CashierWidget::messageReceived(LibG::Message *msg) {
    if (!msg->isSuccess()) {
        if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE)) {
            ui->lineBarcode->selectAll();
            if (Preference::getBool(SETTING::CAI_ENABLE)) {
                if (!mLastBarcode.isEmpty())
                    mAddItemDialog->openBarcode(mLastBarcode);
            } else {
                QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
            }
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
        return;
    }
    if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE)) {
        ui->lineBarcode->clear();
        ui->lineBarcode->setEnabled(true);
        const QString &name = msg->data("item").toMap()["name"].toString();
        const QString &barcode = msg->data("item").toMap()["barcode"].toString();
        const QString &unit = msg->data("item").toMap()["unit"].toString();
        ui->labelName->setText(name);
        const QVariantList &list = msg->data("prices").toList();
        double price = list.first().toMap()["price"].toDouble();
        for (int i = 1; i < list.size(); i++) {
            if (list[i].toMap()["count"].toFloat() == 1.0f) {
                price = list[i].toMap()["price"].toDouble();
                break;
            }
        }
        ui->labelPrice->setText(Preference::formatMoney(price));
        mModel->addItem(mCount, name, barcode, unit, list, msg->data("item").toMap()["flag"].toInt(), QString());
        ui->tableView->resizeRowsToContents();
    } else if (msg->isTypeCommand(MSG_TYPE::SOLD, MSG_COMMAND::NEW_SOLD)) {
        const QVariantMap &data = msg->data();
        mPayCashDialog->hide();
        mAdvancePaymentDialog->hide();
        mPayCashlessDialog->hide();
        if (Preference::getBool(SETTING::PRINTER_CASHIER_KICK))
            openDrawer();
        if (mPayFlag == 0)
            printBill(data);
        PaymentCashSuccessDialog dialog(data, this);
        dialog.exec();
        mModel->reset();
        resetCustomer(true);
        CashierCustomerDisplay::instance()->showNone();
        emit transactionDone();
    } else if (msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY)) {
        const QList<QVariant> &list = msg->data("data").toList();
        if (list.isEmpty()) {
            QMessageBox::critical(this, tr("Error"), tr("Customer not found"));
        } else {
            const QVariantMap &d = list.first().toMap();
            mModel->fillCustomer(d);
            updateCustomerLabel();
        }
    } else if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::INSERT)) {
        if (mAddItemDialog->isAuto())
            ui->lineBarcode->setText(msg->data("barcode").toString());
        mAddItemDialog->hide();
        barcodeEntered();
    }
}

void CashierWidget::cutPaper() {
    if (!Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT))
        return;
    const QString &command = Escp::cutPaperCommand();
    GuiUtil::print(command);
}

void CashierWidget::saveToSlot(int slot) {
    QDir dir = QDir::home();
    dir.mkdir(".sultan");
    dir.cd(".sultan");
    QFile file(dir.absoluteFilePath(QString("trans_%1.trans").arg(slot)));
    if (file.exists())
        file.remove();
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to open save file"));
        return;
    }
    QVariantMap obj;
    obj.insert("customer", mModel->getCustomer()->toMap());
    obj.insert("cart", mModel->getCart());
    const QJsonDocument &doc = QJsonDocument::fromVariant(obj);
    file.write(qCompress(doc.toJson(QJsonDocument::Compact)));
    // file.write(doc.toJson());
    file.close();
}

void CashierWidget::loadFromSlot(int slot) {
    QDir dir = QDir::home();
    dir.mkdir(".sultan");
    dir.cd(".sultan");
    QFile file(dir.absoluteFilePath(QString("trans_%1.trans").arg(slot)));
    if (!file.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("File not exists"));
        return;
    }
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to open file"));
        return;
    }
    QJsonParseError err;
    const QJsonDocument &doc = QJsonDocument::fromJson(qUncompress(file.readAll()), &err);
    if (err.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, tr("Error"), tr("Error parsing json file"));
        return;
    }
    QJsonObject obj = doc.object();
    mModel->loadCart(obj.value("cart").toArray().toVariantList());
    mModel->fillCustomer(obj.value("customer").toObject().toVariantMap());
    updateCustomerLabel();
    ui->tableView->selectRow(mModel->rowCount(QModelIndex()) - 1);
    file.close();
}

void CashierWidget::removeSlot(int slot) {
    QDir dir = QDir::home();
    dir.mkdir(".sultan");
    dir.cd(".sultan");
    QFile file(dir.absoluteFilePath(QString("trans_%1.trans").arg(slot)));
    if (file.exists())
        file.remove();
    mSaveSlot = -1;
}

void CashierWidget::updateCustomerLabel() {
    auto cust = mModel->getCustomer();
    ui->labelCustomerNumber->setText(cust->number.isEmpty() ? tr("None") : cust->number);
    ui->labelCustomerName->setText(cust->name.isEmpty() ? tr("None") : cust->name);
    ui->labelCustomerPoin->setText(QString::number(cust->reward));
}

double CashierWidget::getTax() {
    return Preference::getBool(SETTING::USE_TAX)
               ? Util::calculateDiscount(Preference::getString(SETTING::TAX_VALUE), mModel->getTotal())
               : 0;
}

void CashierWidget::updateItem(CashierItem *item) {
    EditPriceCountDialog dialog(mMessageBus, this);
    dialog.setup(item->barcode, item->count, item->price, item->discount_formula, item->note, item->itemFlag);
    dialog.exec();
    if (dialog.isOk()) {
        QVariantList &prices = mModel->getPrices(item->barcode);
        QVariantMap price = prices[0].toMap();
        price["price"] = dialog.getPrice();
        price["discount_formula"] = dialog.getDiscountFormula();
        prices[0] = price;
        mModel->addItem(dialog.getCount() - item->count, item->name, item->barcode, item->unit, prices, item->itemFlag,
                        dialog.getNote());
        ui->tableView->resizeRowsToContents();
    }
}

void CashierWidget::barcodeWithCtrlPressed() { barcodeEntered(true); }

void CashierWidget::barcodeEntered(bool isControlPressed) {
    QString barcode = ui->lineBarcode->text();
    if (barcode.isEmpty()) {
        payCash();
        return;
    }
    if (barcode.contains("*")) {
        const QStringList ls = barcode.split("*");
        if (ls.size() > 1) {
            barcode = ls[1];
            mCount = ls[0].toFloat();
        }
    } else {
        mCount = 1.0f;
    }
    mLastBarcode = barcode;
    if ((Preference::getBool(SETTING::CASHIER_NAMEBASED) && !isControlPressed) ||
        (!Preference::getBool(SETTING::CASHIER_NAMEBASED) && isControlPressed)) {
        SearchItemDialog dialog(mMessageBus, false, this);
        dialog.setNameField(ui->lineBarcode->text());
        dialog.exec();
        const QString &barcode = dialog.getSelectedBarcode();
        if (barcode.isEmpty())
            return;
        LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE);
        msg.addData("barcode", barcode);
        msg.addFilter("flag", COMPARE::FLAG_ENABLE, ITEM_FLAG::SELLABLE);
        sendMessage(&msg);
    } else {
        LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::CASHIER_PRICE);
        msg.addData("barcode", barcode);
        msg.addFilter("flag", COMPARE::FLAG_ENABLE, ITEM_FLAG::SELLABLE);
        sendMessage(&msg);
    }
}

void CashierWidget::totalChanged(double value) {
    bool isTax = Preference::getBool(SETTING::USE_TAX);
    if (!isTax) {
        ui->labelTotal->setText(Preference::formatMoney(value));
    } else {
        double tax = Util::calculateDiscount(Preference::getString(SETTING::TAX_VALUE), value);
        ui->labelSubTotal->setText(Preference::formatMoney(value));
        ui->labelTax->setText(Preference::formatMoney(tax));
        ui->labelTotal->setText(Preference::formatMoney(value + tax));
    }
}

void CashierWidget::selectRow(const QModelIndex &index) {
    ui->tableView->selectRow(index.row());
    ui->tableView->scrollTo(index);
}

void CashierWidget::tableKeyPressed(QObject * /*sender*/, QKeyEvent *event) {
    const QModelIndex &index = ui->tableView->currentIndex();
    if (!index.isValid())
        return;
    auto item = static_cast<CashierItem *>(index.internalPointer());
    if (event->key() == Qt::Key_Return && !item->isReturn()) {
        updateItem(item);
    } else if (event->key() == Qt::Key_Delete) {
        if (item->isReturn()) {
            mModel->removeReturn(item);
        } else {
            mModel->addItem(-item->count, item->name, item->barcode, item->unit, QVariantList(), item->itemFlag,
                            QString());
        }
    }
}

void CashierWidget::payCash() {
    if (mModel->isEmpty())
        return;
    mPayCashDialog->fill(mModel->getTotal() + getTax());
    mPayCashDialog->show();
    CashierCustomerDisplay::instance()->showTotal();
}

void CashierWidget::payCashless() {
    if (mModel->isEmpty())
        return;
    mPayCashlessDialog->showDialog(mModel->getTotal() + getTax());
}

void CashierWidget::payAdvance() {
    if (mModel->isEmpty())
        return;
    if (!mModel->getCustomer()->isValid()) {
        FlashMessageManager::showMessage(tr("Advance payment only for valid customer"), FlashMessage::Error);
        return;
    }
    mAdvancePaymentDialog->setup(mModel->getTotal() + getTax(), mModel->getCustomer());
    mAdvancePaymentDialog->show();
}

void CashierWidget::openDrawer() {
    const QString &command = Escp::openDrawerCommand();
    GuiUtil::print(command);
}

void CashierWidget::updateCurrentItem() {
    const QModelIndex &index = ui->tableView->currentIndex();
    if (!index.isValid())
        return;
    auto item = static_cast<CashierItem *>(index.internalPointer());
    if (item->isReturn())
        return;
    updateItem(item);
}

void CashierWidget::payRequested(int type, double value, int flag) {
    QVariantMap data;
    mPayFlag = flag;
    double tax = getTax();
    data.insert("number", Util::genSoldNumber());
    data.insert("cart", mModel->getCart());
    data.insert("user_id", UserSession::id());
    data.insert("machine_id", Preference::getInt(SETTING::MACHINE_ID));
    data.insert("subtotal", mModel->getTotal());
    data.insert("tax", tax);
    if (type == PAYMENT::CASH) {
        data.insert("payment", value);
        data.insert("total", mModel->getTotal() + tax);
        data.insert("bank_id", 0);
    } else {
        data.insert("payment", mModel->getTotal());
        data.insert("additional_charge", value);
        data.insert("total", mModel->getTotal() + value + tax);
        data.insert("card_number", mPayCashlessDialog->getCardNumber());
        data.insert("bank_id", mPayCashlessDialog->getBank());
        data.insert("card_type", mPayCashlessDialog->getCardType());
    }
    data.insert("customer_id", mModel->getCustomer()->id);
    data.insert("payment_type", type);
    data.insert("reward", mModel->getRewardPoin());
    Message msg(MSG_TYPE::SOLD, MSG_COMMAND::NEW_SOLD);
    msg.setData(data);
    sendMessage(&msg);
}

void CashierWidget::printBill(const QVariantMap &data) {
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    bool isTax = Preference::getBool(SETTING::USE_TAX);
    if (type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please setting printer first"));
        return;
    }
    int paymentType = data["payment_type"].toInt();
    const QString &title = Preference::getString(SETTING::PRINTER_CASHIER_TITLE, "Sultan Minimarket");
    const QString &subtitle = Preference::getString(SETTING::PRINTER_CASHIER_SUBTITLE, "Jogonalan Lor RT 2 Bantul");
    const QString &footer = Preference::getString(SETTING::PRINTER_CASHIER_FOOTER, "Barang dibeli tidak dapat ditukar");
    bool useBarcode = Preference::getBool(SETTING::PRINTER_CASHIER_SHOW_BARCODE);
    int barcodelen = Preference::getInt(SETTING::PRINTER_CASHIER_BARCODE_LEN, 15);
    int cpi10 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32);
    int cpi12 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40);

    auto escp = new Escp(Escp::SIMPLE, cpi10, cpi12);
    escp->setCpi10Only(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
    escp->cpi10()->doubleHeight(true)->centerText(title)->newLine()->doubleHeight(false)->cpi12();
    if (subtitle.contains("\n")) {
        const QStringList &l = subtitle.trimmed().split("\n");
        for (int i = 0; i < l.size(); i++)
            escp->centerText(l[i])->newLine();
    } else {
        escp->centerText(subtitle);
    }
    escp->newLine(2)
        ->leftText(LibDB::DBUtil::sqlDateToDateTime(data["created_at"].toString()).toString("dd-MM-yy hh:mm"))
        ->newLine();
    escp->fullText(QStringList{data["number"].toString(), UserSession::username()});
    escp->newLine()->column(QList<int>())->line(QChar('='));
    const QVariantList &l = data["cart"].toList();
    for (auto v : l) {
        QVariantMap m = v.toMap();
        int flag = m["flag"].toInt();
        if ((flag & ITEM_FLAG::ITEM_LINK) != 0)
            continue;
        QString name;
        float count = m["count"].toFloat();
        double discount = m["discount"].toDouble();
        const QString &total = Preference::formatMoney(m["final"].toDouble());
        if (useBarcode)
            name = QString("%1 - %2").arg(Util::elide(m["barcode"].toString(), barcodelen)).arg(m["name"].toString());
        else
            name = m["name"].toString();
        if (count == 1.0f && discount == 0 && (name.size() + total.size() + 1) < escp->width()) {
            escp->fullText(QStringList{name, total})->newLine();
            continue;
        }
        const QString &note = m["note"].toString();
        if (name.length() > escp->width())
            name = name.left(escp->width());
        escp->leftText(name)->newLine();
        if (!note.isEmpty())
            escp->leftText(QString("* %1").arg(note))->newLine();
        QString s =
            QString("%1 x %2").arg(Preference::formatFloat(count)).arg(Preference::formatMoney(m["price"].toDouble()));
        if (m["discount"].toDouble() != 0) {
            s = s % " (" % Preference::formatMoney(-discount) % ")";
        }
        escp->fullText(QStringList{s, total})->newLine();
    }
    escp->line()->column(QList<int>());
    if (isTax) {
        escp->fullText(QStringList{tr("Sub-total"), Preference::formatMoney(data["subtotal"].toDouble())})
            ->newLine()
            ->fullText(QStringList{tr("Tax"), Preference::formatMoney(data["tax"].toDouble())})
            ->newLine();
        if (paymentType == PAYMENT::NON_CASH)
            escp->fullText(
                    QStringList{tr("Card Charge"), Preference::formatMoney(data["additional_charge"].toDouble())})
                ->newLine();
        escp->fullText(QStringList{tr("Total"), Preference::formatMoney(data["total"].toDouble())})->newLine();
    } else {
        if (paymentType == PAYMENT::NON_CASH) {
            escp->fullText(QStringList{tr("Sub-total"), Preference::formatMoney(data["subtotal"].toDouble())})
                ->newLine()
                ->fullText(
                    QStringList{tr("Card Charge"), Preference::formatMoney(data["additional_charge"].toDouble())})
                ->newLine();
        }
        escp->fullText(QStringList{tr("Total"), Preference::formatMoney(data["total"].toDouble())})->newLine();
    }
    if (paymentType == PAYMENT::CASH) {
        escp->fullText(QStringList{tr("Payment"), Preference::formatMoney(data["payment"].toDouble())})
            ->newLine()
            ->fullText(QStringList{tr("Change"),
                                   Preference::formatMoney(data["payment"].toDouble() - data["total"].toDouble())})
            ->newLine();
    } else {
        escp->fullText(QStringList{tr("Card Number"), QString("xxxx-%1").arg(data["card_number"].toString().right(4))})
            ->newLine();
    }
    escp->column(QList<int>())->doubleHeight(false);
    if (data.contains("customer")) {
        const QVariantMap &cust = data["customer"].toMap();
        escp->line('=')->leftText(QString("Cust number : %1").arg(cust["number"].toString()))->newLine();
        escp->leftText(QString("Cust Name : %1").arg(cust["name"].toString()))->newLine();
        escp->fullText(QStringList{tr("Reward Poin"), QString::number(cust["reward"].toInt())})->newLine();
        double credit = cust["credit"].toDouble();
        if (credit > 0) {
            escp->fullText(QStringList{tr("Credit"), Preference::formatMoney(cust["credit"].toDouble())})->newLine();
        }
    }
    escp->doubleHeight(false)
        ->line()
        ->leftText(footer, true)
        ->newLine(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    GuiUtil::print(escp->data());
    delete escp;
    cutPaper();
}

void CashierWidget::openSearch() {
    SearchItemDialog dialog(mMessageBus, false, this);
    dialog.exec();
    const QString &barcode = dialog.getSelectedBarcode();
    if (barcode.isEmpty())
        return;
    ui->lineBarcode->setText(QString("%1%2").arg(ui->lineBarcode->text()).arg(barcode));
    barcodeEntered();
}

void CashierWidget::openPreviousTransaction() {
    TransactionListDialog dialog(mMessageBus, this);
    dialog.setPrintFunction(std::bind(&CashierWidget::printBill, this, std::placeholders::_1));
    dialog.exec();
}

void CashierWidget::newTransaction() {
    if (mModel->isEmpty())
        return;
    int res = QMessageBox::question(this, tr("New transaction confirmation"),
                                    tr("Are you sure want to ignore this transaction and start new one?"));
    if (res != QMessageBox::Yes)
        return;
    mModel->reset();
    resetCustomer(true);
    mSaveSlot = -1;
}

void CashierWidget::saveCartTriggered() {
    if (mModel->isEmpty())
        return;
    if (mSaveSlot < 0) {
        SaveLoadSlotDialog dialog(true, this);
        dialog.exec();
        if (dialog.getSelectedSlot() < 0)
            return;
        saveToSlot(dialog.getSelectedSlot());
        mSaveSlot = dialog.getSelectedSlot();
    }
}

void CashierWidget::loadCartTriggered() {
    if (!mModel->isEmpty() && mSaveSlot < 0) {
        int ret = QMessageBox::question(this, tr("Confirmation"),
                                        tr("Your cart is not empty, do you want to ignore current cart?"));
        if (ret != QMessageBox::Yes) {
            return;
        }
    } else if (!mModel->isEmpty() && mSaveSlot >= 0) {
        saveToSlot(mSaveSlot);
    }
    SaveLoadSlotDialog dialog(false, this);
    dialog.exec();
    if (dialog.getSelectedSlot() < 0)
        return;
    loadFromSlot(dialog.getSelectedSlot());
    mSaveSlot = dialog.getSelectedSlot();
}

void CashierWidget::openHelp() {
    CashierHelpDialog dialog(this);
    dialog.exec();
}

void CashierWidget::scanCustomer() {
    const QString &str = QInputDialog::getText(this, tr("Input Customer"), tr("Scan customer ID"));
    if (!str.isEmpty()) {
        Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
        msg.addFilter("number", COMPARE::EQUAL, str);
        sendMessage(&msg);
    }
}

void CashierWidget::resetCustomer(bool dontShowMessage) {
    if (!dontShowMessage)
        FlashMessageManager::showMessage(tr("Customer reseted"));
    mModel->getCustomer()->reset();
    updateCustomerLabel();
}

void CashierWidget::openCheckPrice() {
    QString barcode = "";
    const QModelIndex &index = ui->tableView->currentIndex();
    if (index.isValid() && GuiUtil::isWidgetFocused(ui->tableView)) {
        auto item = static_cast<CashierItem *>(index.internalPointer());
        barcode = item->barcode;
    }
    CheckPriceDialog dialog(mMessageBus, barcode, this);
    dialog.exec();
}

void CashierWidget::openSearchCustomer() {
    SearchCustomerDialog dialog(mMessageBus, this);
    dialog.exec();
    if (dialog.isOk()) {
        Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::QUERY);
        msg.addFilter("number", COMPARE::EQUAL, dialog.getSelectedData()["number"].toString());
        sendMessage(&msg);
    }
}

void CashierWidget::openReport() {
    CashierReportDialog dialog(mMessageBus, this);
    dialog.exec();
}

void CashierWidget::openCustomerCreditPayment() {
    CustomerCreditPaymentDialog dialog(mMessageBus, this);
    dialog.exec();
}

void CashierWidget::focusBarcode() {
    ui->lineBarcode->setFocus();
    ui->lineBarcode->selectAll();
}

void CashierWidget::focusTable() { ui->tableView->setFocus(); }

void CashierWidget::addNonStockTransaction() {
    AddTransactionDialog dialog(mMessageBus, this);
    dialog.exec();
}

void CashierWidget::addNewItem(const QVariantMap &data) {
    LibG::Message msg(MSG_TYPE::ITEM, MSG_COMMAND::INSERT);
    msg.setData(data);
    sendMessage(&msg);
}

void CashierWidget::addNewItemNoBarcode() { mAddItemDialog->openAutoBarcode(); }

void CashierWidget::tableClicked(const QModelIndex &index) {
    if (index.isValid()) {
        ui->tableView->edit(mModel->index(index.row(), 3, QModelIndex()));
    }
}

void CashierWidget::editRequest(const QModelIndex &index, const QVariant &value) {
    auto item = static_cast<CashierItem *>(index.internalPointer());
    QVariantList &prices = mModel->getPrices(item->barcode);
    QVariantMap price = prices[0].toMap();
    price["price"] = item->price;
    price["discount_formula"] = item->discount_formula;
    prices[0] = price;
    mModel->addItem(value.toFloat() - item->count, item->name, item->barcode, item->unit, prices, item->itemFlag,
                    item->note);
    ui->tableView->resizeRowsToContents();
}
