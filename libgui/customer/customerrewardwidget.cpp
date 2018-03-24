/*
 * customerrewardwidget.cpp
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
#include "customerrewardwidget.h"
#include "ui_normalwidget.h"
#include "ui_creditsummarywidget.h"
#include "horizontalheader.h"
#include "tablewidget.h"
#include "tablemodel.h"
#include "tableitem.h"
#include "global_constant.h"
#include "message.h"
#include "tableview.h"
#include "usersession.h"
#include "guiutil.h"
#include "headerwidget.h"
#include "db_constant.h"
#include "flashmessagemanager.h"
#include "preference.h"
#include "addcreditpaymentdialog.h"
#include "dbutil.h"
#include "escp.h"
#include "printer.h"
#include "global_setting_const.h"
#include "addpoindialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

CustomerRewardWidget::CustomerRewardWidget(int id, const QString &number, LibG::MessageBus *bus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalWidget),
    uiSummary(new Ui::CreditSummaryWidget),
    mTableWidget(new TableWidget(this)),
    mId(id),
    mNumber(number)
{
    setMessageBus(bus);
    ui->setupUi(this);
    auto sumWidget = new QWidget(this);
    uiSummary->setupUi(sumWidget);
    ui->labelTitle->setText(tr("Customer Reward : %1").arg(number));
    ui->verticalLayout->addWidget(sumWidget);
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initButton(QList<TableWidget::ButtonType>() << TableWidget::Refresh << TableWidget::Add);
    auto model = mTableWidget->getModel();
    auto dateFormater = [](TableItem *item, const QString &key) {
        return LibDB::DBUtil::sqlDateToDateTime(item->data(key).toString()).toString("dd-MM-yyyy hh:mm");
    };
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date Time"), Qt::AlignLeft, dateFormater);
    model->addColumn("number", tr("Number"));
    model->addColumn("detail", tr("Detail"));
    model->addColumnMoney("reward", tr("Reward"));
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::CUSTOMER_POINT, MSG_COMMAND::QUERY);
    model->setFilter("customer_id", COMPARE::EQUAL, id);
    model->setSort("created_at DESC");
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 300 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    auto button = new QPushButton(QIcon(":/images/16x16/printer.png"), "");
    button->setToolTip(tr("Print"));
    button->setFlat(true);
    mTableWidget->addActionButton(button);
    connect(button, SIGNAL(clicked(bool)), SLOT(printClicked()));
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshCustomer()));
}

void CustomerRewardWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            mTotal = msg->data("credit").toDouble();
            mPoin = msg->data("reward").toInt();
            uiSummary->labelNumber->setText(msg->data("number").toString());
            uiSummary->labelName->setText(msg->data("name").toString());
            uiSummary->labelPhone->setText(msg->data("phone").toString());
            uiSummary->labelAddress->setText(msg->data("address").toString());
            uiSummary->labelReward->setText(QString::number(mPoin));
            uiSummary->labelCredit->setText(Preference::formatMoney(mTotal));
        }
    }
}

void CustomerRewardWidget::addClicked()
{
    AddPoinDialog dialog(mMessageBus, mId, mNumber, mPoin, this);
    dialog.exec();
    mTableWidget->getModel()->refresh();
}

void CustomerRewardWidget::refreshCustomer()
{
    Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::GET);
    msg.addData("id", mId);
    sendMessage(&msg);
}

void CustomerRewardWidget::printClicked()
{
    const QModelIndex &index = mTableWidget->getTableView()->currentIndex();
    if(!index.isValid()) return;
    auto item = static_cast<TableItem*>(index.internalPointer());
    if(item->data("link_id").toInt() != 0) return;
    print(item->data());
}

void CustomerRewardWidget::print(const QVariantMap &data)
{
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    if(type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please setting printer first"));
        return;
    }
    const QString &title = Preference::getString(SETTING::PRINTER_CASHIER_TITLE, "Sultan Minimarket");
    int cpi10 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32);
    int cpi12 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40);

    auto escp = new LibPrint::Escp(LibPrint::Escp::SIMPLE, cpi10, cpi12);
    escp->setCpi10Only(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
    escp->cpi10()->doubleHeight(true)->centerText(title)->newLine()->
            centerText(tr("Reward Exchange"))->
            doubleHeight(false)->cpi12()->newLine(2);
    escp->column(QList<int>())->line(QChar('='));
    escp->column(QList<int>{50, 50})->leftText(tr("Cust-ID"))->rightText(uiSummary->labelNumber->text())->newLine();
    escp->column(QList<int>{50, 50})->leftText(tr("Name"))->rightText(uiSummary->labelName->text())->newLine();
    escp->column(QList<int>{50, 50})->leftText(tr("Total Reward"))->rightText(Preference::formatMoney(mTotal))->newLine();
    escp->column(QList<int>())->line(QChar('-'));
    escp->column(QList<int>{50, 50})->leftText(tr("Date"))->rightText(LibDB::DBUtil::sqlDateToDateTime(data["created_at"].toString()).toString("dd-MM-yy hh:mm"))->newLine();
    escp->leftText(tr("Number"))->rightText(data["number"].toString())->newLine();
    escp->leftText(tr("Reward Exchange"))->rightText(Preference::formatMoney(-data["reward"].toDouble()))->newLine();
    escp->column(QList<int>())->leftText(tr("Detail :"))->newLine()->leftText(data["detail"].toString())->newLine();
    escp->line(QChar('-'))->newLine(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    GuiUtil::print(escp->data());
    delete escp;
    if(Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT)) {
        const QString &command = LibPrint::Escp::cutPaperCommand();
        GuiUtil::print(command);
    }
}
