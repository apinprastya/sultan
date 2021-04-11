/*
 * itemwidget.cpp
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
#include "itemwidget.h"
#include "additemdialog.h"
#include "addpricedialog.h"
#include "db_constant.h"
#include "escp.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "keyevent.h"
#include "message.h"
#include "preference.h"
#include "printer.h"
#include "stockcarddialog.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "tilewidget.h"
#include "ui_itemwidget.h"
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

using namespace LibGUI;
using namespace LibG;

ItemWidget::ItemWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::ItemWidget), mMainTable(new TableWidget(this)),
      mAddDialog(new AddItemDialog(bus, this)), mPriceDialog(new AddPriceDialog(bus, this)),
      mStockValue(new TileWidget(this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    mStockValue->setTitleValue(tr("Stock Value"), tr("loading..."));
    mMainTable->initCrudButton();
    auto model = mMainTable->getModel();
    model->setMessageBus(bus);
    model->addColumn("barcode", tr("Barcode"));
    model->addColumn("name", tr("Name"));
    model->addColumnMoney("buy_price", tr("Buy Price"));
    model->addColumnMoney("sell_price", tr("Sell Price"));
    model->addColumn("stock", tr("Stock"), Qt::AlignRight);
    model->addColumn("unit", tr("Unit"));
    model->addColumn("category", tr("Category"));
    model->addColumn("suplier", tr("Suplier"));
    model->addHeaderFilter("barcode", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLikeNative, QVariant()});
    model->addHeaderFilter("name", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("suplier", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("category", HeaderFilter{HeaderWidget::Combo, TableModel::FilterCategory, QVariant()});
    model->setTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::QUERY);
    mMainTable->setupTable();
    GuiUtil::setColumnWidth(mMainTable->getTableView(), QList<int>()
                                                            << 150 << 150 << 100 << 100 << 150 << 75 << 150 << 150);
    mMainTable->getTableView()->horizontalHeader()->setStretchLastSection(true);
    auto button = new QPushButton(QIcon(":/images/16x16/bagbox.png"), "");
    button->setToolTip(tr("Stock Card"));
    button->setFlat(true);
    connect(button, SIGNAL(clicked(bool)), SLOT(openStockCard()));
    mMainTable->addActionButton(button);
    button = new QPushButton(QIcon(":/images/16x16/drive-download.png"), "");
    button->setToolTip(tr("Export"));
    button->setFlat(true);
    connect(button, SIGNAL(clicked(bool)), SLOT(exportClicked()));
    mMainTable->addActionButton(button);
    button = new QPushButton(QIcon(":/images/16x16/drive-upload.png"), "");
    button->setToolTip(tr("Import"));
    button->setFlat(true);
    connect(button, SIGNAL(clicked(bool)), SLOT(importClicked()));
    mMainTable->addActionButton(button);
    model->refresh();
    auto hor = new QHBoxLayout();
    hor->addWidget(mStockValue);
    hor->addStretch();
    ui->verticalLayoutTop->addLayout(hor);
    ui->verticalLayoutTop->addWidget(mMainTable);

    connect(mMainTable->getTableView()->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            SLOT(mainTableSelectionChanges()));
    connect(mMainTable, SIGNAL(addClicked()), SLOT(addItemClicked()));
    connect(mMainTable, SIGNAL(updateClicked(QModelIndex)), SLOT(updateItemClicked(QModelIndex)));
    connect(mMainTable, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteItemClicked(QModelIndexList)));
    connect(mAddDialog, SIGNAL(success()), mMainTable->getModel(), SLOT(refresh()));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(reloadSummary()));

    auto key = new KeyEvent(this);
    key->addConsumeKey(Qt::Key_P);
    mMainTable->getTableView()->installEventFilter(key);
    connect(key, SIGNAL(keyPressed(QObject *, QKeyEvent *)), SLOT(mainTableKeyPressed(QObject *, QKeyEvent *)));

    Message msg(MSG_TYPE::CATEGORY, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

ItemWidget::~ItemWidget() { delete ui; }

void ItemWidget::messageReceived(LibG::Message *msg) {
    if (msg->isType(MSG_TYPE::CATEGORY) && msg->isSuccess()) {
        const QVariantList &list = msg->data("data").toList();
        GuiUtil::populateCombo(
            mMainTable->getTableView()->getHeaderWidget(mMainTable->getModel()->getIndex("category"))->getComboBox(),
            list, tr("-- Select Category --"));
    } else if (msg->isTypeCommand(MSG_TYPE::SELLPRICE, MSG_COMMAND::DEL) && msg->isSuccess()) {
        FlashMessageManager::showMessage(tr("Price deleted successfully"));
        // mSecondTable->getModel()->refresh();
    } else if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::DEL) && msg->isSuccess()) {
        FlashMessageManager::showMessage(tr("Item deleted successfully"));
        mMainTable->getModel()->refresh();
    } else if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::EXPORT)) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save as CSV"), QDir::homePath(), "*.csv");
        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".csv"))
                fileName += ".csv";
            QFile file(fileName);
            if (file.open(QFile::WriteOnly)) {
                file.write(msg->data("data").toString().toUtf8());
                file.close();
            } else {
                QMessageBox::critical(this, tr("Error"), tr("Unable to save to file"));
            }
        }
    } else if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::IMPORT) && msg->isSuccess()) {
        FlashMessageManager::showMessage(tr("Item imported successfully"));
    } else if (msg->isTypeCommand(MSG_TYPE::ITEM, MSG_COMMAND::SUMMARY)) {
        mStockValue->setValue(Preference::formatMoney(msg->data("total").toDouble()));
    }
}

void ItemWidget::mainTableSelectionChanges() {
    const QModelIndex &index = mMainTable->getTableView()->currentIndex();
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        mCurrentBarcode = item->data("barcode").toString();
        mCurrentName = item->data("name").toString();
        mCurrentBuyPrice = item->data("buy_price").toDouble();
    }
}

void ItemWidget::addItemClicked() {
    mAddDialog->reset();
    mAddDialog->show();
}

void ItemWidget::updateItemClicked(const QModelIndex &index) {
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    mAddDialog->reset();
    mAddDialog->setAsUpdate();
    mAddDialog->openBarcode(item->data("barcode").toString());
    mAddDialog->show();
}

void ItemWidget::deleteItemClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    int ret = QMessageBox::question(this, tr("Confirmation"),
                                    tr("The stocks cards and item link will be removed. Are you sure to delete item?"));
    if (ret != QMessageBox::Yes)
        return;
    QList<QVariant> ids;
    for (int i = 0; i < index.size(); i++) {
        auto item = static_cast<TableItem *>(index[i].internalPointer());
        ids.append(item->data("barcode"));
    }
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::DEL);
    msg.addData("barcode", ids);
    sendMessage(&msg);
}

void ItemWidget::importClicked() {
    int res = QMessageBox::question(
        this, tr("Confirmation"),
        tr("Your current item, category and supplier will be wipe out. Sure to continue import?"));
    if (res == QMessageBox::Yes) {
        const QString &fileName = QFileDialog::getOpenFileName(this, tr("Import items"), QDir::homePath(), "*.csv");
        if (fileName.isEmpty())
            return;
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly))
            return;
        Message msg(MSG_TYPE::ITEM, MSG_COMMAND::IMPORT);
        msg.addData("data", QString::fromUtf8(file.readAll()));
        sendMessage(&msg);
    }
}

void ItemWidget::exportClicked() {
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::EXPORT);
    msg.addData("version", qApp->applicationVersion());
    sendMessage(&msg);
}

void ItemWidget::mainTableKeyPressed(QObject * /*sender*/, QKeyEvent *event) {
    if (event->key() == Qt::Key_P) {
        const QModelIndex &index = mMainTable->getTableView()->currentIndex();
        if (index.isValid()) {
            auto item = static_cast<TableItem *>(index.internalPointer());
            printPrice(item);
        }
    }
}

void ItemWidget::openStockCard() {
    const QModelIndex &index = mMainTable->getTableView()->currentIndex();
    if (index.isValid()) {
        auto item = static_cast<TableItem *>(index.internalPointer());
        StockCardDialog dialog(item->data("barcode").toString(), mMessageBus);
        dialog.exec();
    }
}

void ItemWidget::reloadSummary() {
    Message msg(MSG_TYPE::ITEM, MSG_COMMAND::SUMMARY);
    sendMessage(&msg);
}

void ItemWidget::printPrice(TableItem *item) {
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    if (type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please setting printer first"));
        return;
    }
    int cpi10 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32);
    int cpi12 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40);

    auto escp = new LibPrint::Escp(LibPrint::Escp::SIMPLE, cpi10, cpi12);
    escp->setCpi10Only(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
    escp->cpi10()
        ->line(QChar('='))
        ->newLine()
        ->centerText(item->data("name").toString())
        ->newLine()
        ->centerText(Preference::formatMoney(item->data("sell_price").toDouble()))
        ->newLine()
        ->line(QChar('='))
        ->newLine(Preference::getInt(SETTING::PRINTER_CASHIER_PRICE_LINEFEED, 2));
    GuiUtil::print(escp->data());
    delete escp;
    if (Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT)) {
        const QString &command = LibPrint::Escp::cutPaperCommand();
        GuiUtil::print(command);
    }
}
