/*
 * customercredit.cpp
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
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
#include "customercreditwidget.h"
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
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;

CustomerCreditWidget::CustomerCreditWidget(int id, const QString &number, LibG::MessageBus *bus, QWidget *parent):
    QWidget(parent),
    ui(new Ui::NormalWidget),
    uiSummary(new Ui::CreditSummaryWidget),
    mTableWidget(new TableWidget(this)),
    mId(id)
{
    setMessageBus(bus);
    ui->setupUi(this);
    auto sumWidget = new QWidget(this);
    uiSummary->setupUi(sumWidget);
    ui->labelTitle->setText(tr("Customer Credit : %1").arg(number));
    ui->verticalLayout->addWidget(sumWidget);
    ui->verticalLayout->addWidget(mTableWidget);
    mTableWidget->initCrudButton();
    auto model = mTableWidget->getModel();
    model->setMessageBus(bus);
    model->addColumn("created_at", tr("Date Time"));
    model->addColumn("number", tr("Number"));
    model->addColumn("detail", tr("Detail"));
    model->addColumn("credit", tr("Credit"));
    model->addHeaderFilter("number", HeaderFilter{HeaderWidget::LineEdit, TableModel::FilterLike, QVariant()});
    model->setTypeCommand(MSG_TYPE::CUSTOMER_CREDIT, MSG_COMMAND::QUERY);
    model->setFilter("customer_id", COMPARE::EQUAL, id);
    mTableWidget->setupTable();
    GuiUtil::setColumnWidth(mTableWidget->getTableView(), QList<int>() << 150 << 150 << 150 << 150);
    mTableWidget->getTableView()->horizontalHeader()->setStretchLastSection(true);
    model->refresh();
    connect(mTableWidget, SIGNAL(addClicked()), SLOT(addClicked()));
    connect(mTableWidget, SIGNAL(updateClicked(QModelIndex)), SLOT(updateClicked(QModelIndex)));
    connect(mTableWidget, SIGNAL(deleteClicked(QModelIndex)), SLOT(deleteClicked(QModelIndex)));
    connect(model, SIGNAL(firstDataLoaded()), SLOT(refreshCustomer()));
}

CustomerCreditWidget::~CustomerCreditWidget()
{
}

void CustomerCreditWidget::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::CUSTOMER, MSG_COMMAND::GET)) {
        if(msg->isSuccess()) {
            uiSummary->labelNumber->setText(msg->data("number").toString());
            uiSummary->labelName->setText(msg->data("name").toString());
            uiSummary->labelPhone->setText(msg->data("phone").toString());
            uiSummary->labelAddress->setText(msg->data("address").toString());
            uiSummary->labelReward->setText(QString::number(msg->data("reward").toInt()));
            uiSummary->labelCredit->setText(Preference::toString(msg->data("credit").toDouble()));
        }
    }
}

void CustomerCreditWidget::addClicked()
{

}

void CustomerCreditWidget::updateClicked(const QModelIndex &index)
{

}

void CustomerCreditWidget::deleteClicked(const QModelIndex &index)
{

}

void CustomerCreditWidget::refreshCustomer()
{
    Message msg(MSG_TYPE::CUSTOMER, MSG_COMMAND::GET);
    msg.addData("id", mId);
    sendMessage(&msg);
}
