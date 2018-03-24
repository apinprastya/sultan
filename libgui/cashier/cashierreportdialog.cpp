/*
 * cashierreportdialog.cpp
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
#include "cashierreportdialog.h"
#include "ui_cashierreportdialog.h"
#include "global_constant.h"
#include "message.h"
#include "preference.h"
#include "global_setting_const.h"
#include "escp.h"
#include "guiutil.h"
#include <QDateTime>
#include <QStringBuilder>
#include <QMessageBox>
#include <QDebug>

using namespace LibGUI;
using namespace LibG;
using namespace LibPrint;

CashierReportDialog::CashierReportDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CashierReportDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);
    ui->textBrowser->setHtml("Loading...");
    auto now = QDateTime::currentDateTime();
    now.setTime(QTime(0, 0, 0));
    ui->dateTimeStart->setDateTime(now);
    auto now2 = now.addDays(1);
    ui->dateTimeEnd->setDateTime(now2);
    search();
    connect(ui->pushFilter, SIGNAL(clicked(bool)), SLOT(search()));
    connect(ui->pushPrint, SIGNAL(clicked(bool)), SLOT(print()));
}

CashierReportDialog::~CashierReportDialog()
{
    delete ui;
}

void CashierReportDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isTypeCommand(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY)) {
        mData = msg->data("data").toList();
        QString html = "<div style='text-align: center'><h3>" % tr("Cashier Report") % "</h3><div>" % Preference::getString(SETTING::MACHINE_NAME) %"</div></div>";
        if(mData.isEmpty()) {
            ui->textBrowser->setHtml(tr("Data is empty"));
        } else {
            ui->pushPrint->setEnabled(true);
            for(int i = 0; i < mData.size(); i++) {
                const QVariantMap &d = mData[i].toMap();
                const QVariantList &t = d["type"].toList();
                const QVariantList &b = d["bank"].toList();
                QString det = tr("<div><br><hr><b>User : ") % d["user"].toString() % "</b><br><br><b><i>" % tr("By Type") % "</i></b>";
                QString table = "<table border='0' width='100%'></table>";
                for(int j = 0; j < t.size(); j++) {
                    const QVariantMap &d1 = t[j].toMap();
                    int type = d1["link_type"].toInt();
                    table = table % QString("<tr><td width='40%'>%1</td><td align='right' width='30%'>%2</td><td></td></tr>").
                            arg(getType(type)).arg(Preference::formatMoney(d1["total"].toDouble()));
                }
                table = table % "</table>";
                det = det % table % "<br><br><b><i>" % tr("By Bank") % "</i></b>";
                QString table2 = "<table border='0' width='100%'></table>";
                for(int j = 0; j < b.size(); j++) {
                    const QVariantMap &d1 = b[j].toMap();
                    const QString &bank = d1["bank"].toString();
                    table2 = table2 % QString("<tr><td width='40%'>%1</td><td align='right' width='30%'>%2</td><td></td></tr>").
                            arg(bank.isEmpty() ? tr("Cash") : bank).arg(Preference::formatMoney(d1["total"].toDouble()));
                }
                table2 = table2 % "</table>";                
                det = det % table2;
                det = det % "</div>";
                html += det;
            }
            ui->textBrowser->setHtml(html);
        }
    }
}

QString CashierReportDialog::getType(int type)
{
    switch (type) {
    case TRANSACTION_LINK_TYPE::TRANSACTION:
        return tr("Transaction");
    case TRANSACTION_LINK_TYPE::SOLD:
        return tr("Sold");
    case TRANSACTION_LINK_TYPE::PURCHASE:
        return tr("Purchase");
    case TRANSACTION_LINK_TYPE::CUSTOMER_CREDIT:
        return tr("Customer Credit");
    case TRANSACTION_LINK_TYPE::BUY_RETURN:
        return tr("Buy Return");
    case TRANSACTION_LINK_TYPE::SOLD_RETURN:
        return tr("Sold Return");
    }
    return QString();
}

void CashierReportDialog::search()
{
    Message msg(MSG_TYPE::TRANSACTION, MSG_COMMAND::SUMMARY);
    msg.addData("machine_id", Preference::getInt(SETTING::MACHINE_ID));
    msg.addData("start", ui->dateTimeStart->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    msg.addData("end", ui->dateTimeEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    sendMessage(&msg);
    ui->pushPrint->setEnabled(false);
}

void CashierReportDialog::print()
{
    int type = Preference::getInt(SETTING::PRINTER_CASHIER_TYPE, -1);
    if(type < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please setting printer first"));
        return;
    }
    int cpi10 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI10, 32);
    int cpi12 = Preference::getInt(SETTING::PRINTER_CASHIER_CPI12, 40);

    auto escp = new Escp(Escp::SIMPLE, cpi10, cpi12);
    escp->setCpi10Only(Preference::getBool(SETTING::PRINTER_CASHIER_ONLY_CPI10));
    escp->cpi10()->doubleHeight(true)->centerText(tr("Cashier Report"))->newLine();
    escp->cpi12()->doubleHeight(false)->centerText(Preference::getString(SETTING::MACHINE_NAME))->newLine();
    escp->line();
    escp->leftText(tr("From : %1").arg(ui->dateTimeStart->dateTime().toString("dd-MM-yyyy hh:mm:ss")))->newLine();
    escp->leftText(tr("To   : %1").arg(ui->dateTimeEnd->dateTime().toString("dd-MM-yyyy hh:mm:ss")))->newLine();
    escp->line(QChar('='));
    for(int i = 0; i < mData.size(); i++) {
        const QVariantMap &d = mData[i].toMap();
        const QVariantList &t = d["type"].toList();
        const QVariantList &b = d["bank"].toList();
        escp->column(QList<int>());
        escp->bold(true)->leftText(tr("User : %1").arg(d["user"].toString()))->newLine()->newLine();
        escp->leftText(tr("By Type"))->bold(false)->newLine();
        for(int j = 0; j < t.size(); j++) {
            const QVariantMap &d1 = t[j].toMap();
            int type = d1["link_type"].toInt();
            escp->column(QList<int>{50, 50})->leftText(getType(type))->rightText(Preference::formatMoney(d1["total"].toDouble()))->newLine();
        }
        escp->column(QList<int>());
        escp->newLine()->bold(true)->leftText(tr("By Bank"))->bold(false)->newLine();
        for(int j = 0; j < b.size(); j++) {
            const QVariantMap &d1 = b[j].toMap();
            const QString &bank = d1["bank"].toString();
            escp->column(QList<int>{50, 50})->leftText(bank.isEmpty() ? tr("Cash") : bank)->
                    rightText(Preference::formatMoney(d1["total"].toDouble()))->newLine();
        }
        escp->column(QList<int>());
        escp->line()->newLine();
    }
    escp->newLine(Preference::getInt(SETTING::PRINTER_CASHIER_LINEFEED, 3));
    GuiUtil::print(escp->data());
    if(Preference::getBool(SETTING::PRINTER_CASHIER_AUTOCUT)) {
        const QString &command = Escp::cutPaperCommand();
        GuiUtil::print(command);
    }
    delete escp;
}
