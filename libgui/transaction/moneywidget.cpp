/*
 * moneywidget.cpp
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
#include "moneywidget.h"
#include "ui_normalwidget.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableview.h"
#include "tableitem.h"
#include "global_constant.h"
#include "guiutil.h"
#include "db_constant.h"
#include "message.h"
#include "headerwidget.h"
#include "tilewidget.h"
#include "message.h"
#include "preference.h"
#include "dbutil.h"
#include "util.h"
#include <QHBoxLayout>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

MoneyWidget::MoneyWidget(LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    mTableWidget(new TableWidget(this)),
    mTileTotal(new TileWidget(this))
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->labelTitle->setText(tr("Money"));

    auto hor = new QHBoxLayout;
    mTileTotal->setTitleValue(tr("Total"), "loading...");
    hor->addWidget(mTileTotal);
    hor->addStretch();
    ui->verticalLayout->addLayout(hor);
    ui->verticalLayout->addWidget(mTableWidget);

    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh);
    auto model = mTableWidget->getModel();
    model->setDateTimeISO(true);
    model->setMessageBus(bus);
    model->addColumn("date", tr("Date"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    });
    model->addColumn("link_type", tr("Type"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        switch(item->data(key).toInt()) {
        case TRANSACTION_LINK_TYPE::TRANSACTION: return tr("Transaction");
        case TRANSACTION_LINK_TYPE::SOLD: return tr("Sold");
        case TRANSACTION_LINK_TYPE::PURCHASE: return tr("Purchase");
        case TRANSACTION_LINK_TYPE::CUSTOMER_CREDIT: return tr("Credit");
        case TRANSACTION_LINK_TYPE::BUY_RETURN: return tr("Purchase return");
        case TRANSACTION_LINK_TYPE::SOLD_RETURN: return tr("Sold return");
        }
        return QString();
    });
    model->addColumn("number", tr("Number"));
    model->addColumn("bank", tr("Bank"), Qt::AlignLeft, [](TableItem *item, const QString &key) {
        const QString &str = item->data(key).toString();
        if(str.isEmpty()) return tr("Cash");
        else return str;
    });
    model->addColumn("machine", tr("Machine"));
    model->addColumn("user", tr("User"));
    model->addColumn("detail", tr("Detail"));
    model->addColumnMoney("money_total", tr("Total"));
    QVariantMap defVal;
    defVal.insert("start", Util::getBeginningOfMonth());
    defVal.insert("end", Util::getEndOfMonth());
    model->addHeaderFilter("date", HeaderFilter{HeaderWidget::DateStartEnd, TableModel::FilterBetweenDate, defVal});
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("machine", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("user", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("detail", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->addHeaderFilter("link_type", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->addHeaderFilter("bank", HeaderFilter{HeaderWidget::Combo, TableModel::FilterEQ, QVariant()});
    model->setTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::QUERY);
    model->setFilter("money_total", COMPARE::NEQUAL, 0);
    model->setSort("date DESC");
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 100 << 200 << 100 << 100 << 100 << 250 << 100);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    //model->refresh();
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshSummary()));
    Message msg(MSG_TYPE::BANK, MSG_COMMAND::QUERY);
    sendMessage(&msg);
}

void LibGUI::MoneyWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_MONEY)) {
        if(msg->isSuccess()) {
            mTileTotal->setValue(Preference::formatMoney(msg->data("total").toDouble()));
        }
    } else if(msg->isTypeCommand(MSG_TYPE::BANK, MSG_COMMAND::QUERY)) {
        if(msg->isSuccess()) {
            auto combo = mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("bank"))->getComboBox();
            combo->blockSignals(true);
            combo->addItem(tr("All"), -1);
            combo->addItem(tr("Cash"), 0);
            const QVariantList &l = msg->data("data").toList();
            for(int i = 0; i < l.size(); i++) {
                const QVariantMap &m = l[i].toMap();
                combo->addItem(m["name"].toString(), m["id"]);
            }
            combo->blockSignals(false);
        }
    }
}

void LibGUI::MoneyWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if(isShowed) return;
    isShowed = true;
    auto combo = mTableWidget->getTableView()->getHeaderWidget(mTableWidget->getModel()->getIndex("link_type"))->getComboBox();
    combo->clear();
    combo->addItem(tr("All"), -1);
    combo->addItem(tr("Transaction"), TRANSACTION_LINK_TYPE::TRANSACTION);
    combo->addItem(tr("Sold"), TRANSACTION_LINK_TYPE::SOLD);
    combo->addItem(tr("Purchase"), TRANSACTION_LINK_TYPE::PURCHASE);
    combo->addItem(tr("Credit"), TRANSACTION_LINK_TYPE::CUSTOMER_CREDIT);
    combo->addItem(tr("Purchase return"), TRANSACTION_LINK_TYPE::BUY_RETURN);
    combo->addItem(tr("Sold return"), TRANSACTION_LINK_TYPE::SOLD_RETURN);
}

void LibGUI::MoneyWidget::refreshSummary()
{
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY_MONEY);
    auto query = mTableWidget->getModel()->getQuery();
    query->bind(&msg);
    sendMessage(&msg);
}
