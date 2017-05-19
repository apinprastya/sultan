/*
 * purchaseadddialog.cpp
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
#include "purchaseadddialog.h"
#include "ui_purchaseadddialog.h"
#include "message.h"
#include "global_constant.h"
#include "guiutil.h"
#include "dbutil.h"
#include <QDebug>
#include <QMessageBox>

using namespace LibGUI;
using namespace LibG;

PurchaseAddDialog::PurchaseAddDialog(LibG::MessageBus *bus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseAddDialog)
{
    ui->setupUi(this);
    setMessageBus(bus);
    ui->dateCreated->setCalendarPopup(true);
    ui->dateDeadline->setCalendarPopup(true);
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(saveClicked()));
}

PurchaseAddDialog::~PurchaseAddDialog()
{
    delete ui;
}

void PurchaseAddDialog::reset()
{
    ui->dateCreated->setDate(QDate::currentDate());
    ui->dateDeadline->setDate(QDate::currentDate().addDays(7));
    ui->lineNumber->clear();
    ui->comboSuplier->setCurrentIndex(0);
    mCurrentSuplier = 0;
    ui->dateCreated->setFocus(Qt::TabFocusReason);
    mId = 0;
    ui->pushSave->setEnabled(true);
}

void PurchaseAddDialog::fill(const QVariantMap &data)
{
    ui->dateCreated->setDate(LibDB::DBUtil::sqlDateToDate(data["created_at"].toString()));
    ui->dateDeadline->setDate(LibDB::DBUtil::sqlDateToDate(data["deadline"].toString()));
    ui->lineNumber->setText(data["number"].toString());
    mCurrentSuplier = data["suplier_id"].toInt();
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
    ui->dateCreated->setFocus(Qt::TabFocusReason);
    ui->pushSave->setEnabled(true);
    mId = data["id"].toInt();
}

void PurchaseAddDialog::messageReceived(LibG::Message *msg)
{
    if(msg->isType(MSG_TYPE::SUPLIER)) {
        const QVariantList &list = msg->data("data").toList();
        populateSuplier(list);
    } else if(msg->isType(MSG_TYPE::PURCHASE)) {
        if(msg->isSuccess()) {
            hide();
            if(mId > 0)
                emit successUpdate(mId);
            else
                emit successAdd();
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
    }
}

void PurchaseAddDialog::showEvent(QShowEvent *event)
{
    Message msg(MSG_TYPE::SUPLIER, MSG_COMMAND::QUERY);
    msg.setLimit(-1);
    sendMessage(&msg);
    QDialog::showEvent(event);
}

void PurchaseAddDialog::populateSuplier(const QVariantList &list)
{
    ui->comboSuplier->clear();
    ui->comboSuplier->addItem(tr("-- Select Suplier --"), 0);
    for(auto &d : list) {
        const QVariantMap &m = d.toMap();
        ui->comboSuplier->addItem(m["name"].toString(), m["id"].toInt());
    }
    GuiUtil::selectCombo(ui->comboSuplier, mCurrentSuplier);
}

void PurchaseAddDialog::saveClicked()
{
    if(GuiUtil::anyEmpty(QList<QWidget*>() << ui->lineNumber << ui->comboSuplier)) {
        QMessageBox::critical(this, tr("Error"), tr("Please fill required field"));
        return;
    }
    QVariantMap d;
    d.insert("created_at", ui->dateCreated->date().toString("yyyy-MM-dd"));
    d.insert("number", ui->lineNumber->text());
    d.insert("suplier_id", ui->comboSuplier->currentData());
    d.insert("deadline", ui->dateDeadline->date().toString("yyyy-MM-dd"));
    Message msg(MSG_TYPE::PURCHASE, MSG_COMMAND::INSERT);
    if(mId > 0) {
        msg.setCommand(MSG_COMMAND::UPDATE);
        msg.addData("id", mId);
        msg.addData("data", d);
    } else {
        msg.setData(d);
    }
    sendMessage(&msg);
    ui->pushSave->setEnabled(false);
}
