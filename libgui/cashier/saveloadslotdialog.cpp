/*
 * saveloadslotdialog.cpp
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
#include "saveloadslotdialog.h"
#include "ui_saveloadslotdialog.h"
#include "keyevent.h"
#include <QFile>
#include <QMessageBox>
#include <QDir>

using namespace LibGUI;

SaveLoadSlotDialog::SaveLoadSlotDialog(bool isSave, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveLoadSlotDialog),
    mIsSave(isSave)
{
    ui->setupUi(this);
    auto keyEvent = new KeyEvent(ui->tableWidget);
    keyEvent->addConsumeKey(Qt::Key_Return);
    ui->tableWidget->installEventFilter(keyEvent);
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->verticalHeader()->hide();
    auto headerItem = new QTableWidgetItem(tr("Slot"));
    ui->tableWidget->setHorizontalHeaderItem(0, headerItem);
    headerItem = new QTableWidgetItem(tr("Available"));
    ui->tableWidget->setHorizontalHeaderItem(1, headerItem);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    QDir dir = QDir::home();
    dir.mkdir(".sultan");
    dir.cd(".sultan");
    for(int i = 0; i < 10; i++) {
        auto item = new QTableWidgetItem(QString::number(i + 1));
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 0, item);
        item = new QTableWidgetItem(isSave ? tr("YES") : tr("NO"));
        item->setData(Qt::UserRole, 1);
        item->setTextAlignment(Qt::AlignCenter);
        QFile file(dir.absoluteFilePath(QString("trans_%1.trans").arg(i)));
        if(file.exists()){
            item->setText(isSave ? tr("NO") : tr("YES"));
            item->setData(Qt::UserRole, 0);
        }
        ui->tableWidget->setItem(i, 1, item);
    }
    ui->tableWidget->selectRow(0);
    if(!isSave) ui->pushButton->setText(tr("Load"));
    connect(keyEvent, SIGNAL(keyPressed(QObject*,QKeyEvent*)), SLOT(returnPressed()));
    connect(ui->pushButton, SIGNAL(clicked(bool)), SLOT(returnPressed()));
}

SaveLoadSlotDialog::~SaveLoadSlotDialog()
{
    delete ui;
}

void SaveLoadSlotDialog::returnPressed()
{
    auto item = ui->tableWidget->item(ui->tableWidget->currentRow(), 1);
    int available = item->data(Qt::UserRole).toInt();
    if(!available && mIsSave) {
        int ret = QMessageBox::question(this, tr("Override Confirmation"), tr("Slot is in used, are you sure to override?"));
        if(ret != QMessageBox::Yes) {
            return;
        }
    } else if(available && !mIsSave){
        return;
    }
    mSelectedSlot = ui->tableWidget->currentRow();
    close();
}
