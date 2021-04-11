/*
 * transactionwidget.cpp
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
#include "transactionwidget.h"
#include "addtransactiondialog.h"
#include "db_constant.h"
#include "dbutil.h"
#include "flashmessagemanager.h"
#include "global_constant.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "message.h"
#include "preference.h"
#include "tableitem.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tablewidget.h"
#include "tilewidget.h"
#include "ui_normalwidget.h"
#include "util.h"
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

using namespace LibGUI;
using namespace LibG;

TransactionWidget::TransactionWidget(LibG::MessageBus *bus, QWidget *parent)
    : QWidget(parent), ui(new Ui::NormalWidget), mTableWidget(new TableWidget(this)), mTileIncome(new TileWidget(this)),
      mTileExpense(new TileWidget(this)), mTileDifference(new TileWidget(this)), mTileNet(new TileWidget(this)),
      mTileProfit(new TileWidget(this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Transactions"));

    auto hor = new QHBoxLayout;
    mTileIncome->setTitleValue(tr("Bruto"), "loading...");
    hor->addWidget(mTileIncome);
    mTileExpense->setTitleValue(tr("Expense"), "loading...");
    hor->addWidget(mTileExpense);
    mTileDifference->setTitleValue(tr("Bruto - Expense"), "loading...");
    hor->addWidget(mTileDifference);
    auto line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setLineWidth(1);
    line->setFrameShadow(QFrame::Plain);
    hor->addWidget(line);
    mTileNet->setTitleValue(tr("Net"), "loading...");
    hor->addWidget(mTileNet);
    mTileProfit->setTitleValue(tr("Profit"), "loading...");
    hor->addWidget(mTileProfit);
    hor->addStretch();
    ui->verticalLayout->addLayout(hor);
    ui->verticalLayout->addWidget(mTableWidget);

    // mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh << TableWidget::Add);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setDateTimeISO(true);
    model->setMessageBus(bus);
    model->addColumn("date", tr("Date"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    });
    model->addColumn("type", tr("Type"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return item->data(key).toInt() == TRANSACTION_TYPE::INCOME ? tr("Income") : tr("Expense");
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("detail", tr("Detail"));
    model->addColumnMoney("transaction_total", tr("Total"));
    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("date", HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("detail", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("type", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->setTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::QUERY);
    model->setFilter("transaction_total", COMPARE::NEQUAL, 0);
    model->setSort("date DESC");
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 100 << 200 << 300 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    auto button = new QPushButton(QIcon(":/images/16x16/drive-download.png"), "");
    button->setToolTip(tr("Export"));
    button->setFlat(true);
    connect(button, SIGNAL(clicked(bool)), SLOT(exportClicked()));
    mTableWidget->addActionButton(button);
    // model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(editClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndexList)), SLOT(deleteClicked(QModelIndexList)));
}

void TransactionWidget::messageReceived(LibG::Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_TRANSACTION)) {
        if (msg->isSuccess()) {
            double expense = msg->data("expense").toDouble();
            double net = msg->data("margin").toDouble() + msg->data("transonly").toDouble();
            mTileIncome->setValue(Preference::formatMoney(msg->data("income").toDouble()));
            mTileExpense->setValue(Preference::formatMoney(expense));
            mTileDifference->setValue(Preference::formatMoney(msg->data("total").toDouble()));
            mTileNet->setValue(Preference::formatMoney(net));
            mTileProfit->setValue(Preference::formatMoney(net + expense));
        }
    } else if (msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::DEL)) {
        FlashMessageManager::showMessage(tr("Suplier deleted successfully"));
        mTableWidget->getModel()->refresh();
    } else if (msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::EXPORT)) {
        if (msg->isSuccess()) {
            const QString &fileName = QFileDialog::getSaveFileName(this, tr("Save as CSV"), QDir::homePath(), "*.csv");
            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QFile::WriteOnly)) {
                    file.write(msg->data("data").toString().toUtf8());
                    file.close();
                } else {
                    QMessageBox::critical(this, tr("Error"), tr("Unable to save to file"));
                }
            }
        }
    }
}

void TransactionWidget::showEvent(QShowEvent *e) {
    QWidget::showEvent(e);
    if (isShowed)
        return;
    isShowed = true;
    auto combo =
        mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("type"))->getComboBox();
    combo->blockSignals(true);
    combo->clear();
    combo->addItem(tr("All"), -1);
    combo->addItem(tr("Income"), TRANSACTION_TYPE::INCOME);
    combo->addItem(tr("Expense"), TRANSACTION_TYPE::EXPENSE);
    combo->blockSignals(false);
}

void TransactionWidget::refreshSummary() {
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_TRANSACTION);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}

void TransactionWidget::addClicked() {
    AddTransactionDialog dialog(mMessageBus, this);
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void TransactionWidget::editClicked(const QModelIndex &index) {
    if (!index.isValid())
        return;
    auto item = static_cast<TableItem *>(index.internalPointer());
    if (item->data("link_type").toInt() != TRANSACTION_LINK_TYPE::TRANSACTION) {
        QMessageBox::critical(this, tr("Error"), tr("Can not edit transaction from cashier, use sale return instead"));
        return;
    }
    AddTransactionDialog dialog(mMessageBus, this);
    dialog.fill(item->data());
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void TransactionWidget::deleteClicked(const QModelIndexList &index) {
    if (index.empty())
        return;
    QVariantList ids;
    for (int i = 0; i < index.size(); i++) {
        auto item = static_cast<TableItem *>(index[i].internalPointer());
        ids.append(item->id);
        if (item->data("link_type").toInt() != TRANSACTION_LINK_TYPE::TRANSACTION) {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Can not remote transaction from cashier, use sale return instead"));
            return;
        }
    }
    int ret = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure want to remove transaction?"));
    if (ret == QMessageBox::Yes) {
        Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::DEL);
        msg.addData("id", ids);
        sendMessage(&msg);
    }
}

void TransactionWidget::exportClicked() {
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::EXPORT);
    sendMessage(&msg);
}
