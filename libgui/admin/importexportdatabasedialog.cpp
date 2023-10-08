/*
 * importexportdatabasedialog.cpp
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
#include "importexportdatabasedialog.h"
#include "browserdialog.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "guiutil.h"
#include "listdialog.h"
#include "message.h"
#include "preference.h"
#include "ui_importexportdatabasedialog.h"
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

using namespace LibGUI;
using namespace LibG;

ImportExportDatabaseDialog::ImportExportDatabaseDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::ImportExportDatabaseDialog) {
    ui->setupUi(this);
    setMessageBus(bus);
    adjustSize();
    connect(ui->pushExportToFile, SIGNAL(clicked(bool)), SLOT(exportFile()));
    connect(ui->pushImportFile, SIGNAL(clicked(bool)), SLOT(importFile()));
    ui->pushExportToGDrive->hide();
    ui->pushImportGDrive->hide();
}

ImportExportDatabaseDialog::~ImportExportDatabaseDialog() { delete ui; }

void ImportExportDatabaseDialog::messageReceived(Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::DATABASE, MSG_COMMAND::EXPORT)) {
        if (msg->isSuccess()) {
            const QString b64 = msg->data("data").toString();
            const QByteArray &ba = QByteArray::fromBase64(b64.toUtf8());
            auto filename =
                QFileDialog::getSaveFileName(this, tr("Save exported database"), QDir::homePath(), "*.sltn");
            if (!filename.isEmpty()) {
                if (!filename.endsWith(".sltn"))
                    filename += ".sltn";
                QFile file(filename);
                if (file.open(QFile::WriteOnly)) {
                    file.write(ba);
                    file.close();
                }
            }
            GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive,
                                                         ui->pushImportFile, ui->pushImportGDrive});
        }
    } else if (msg->isTypeCommand(MSG_TYPE::DATABASE, MSG_COMMAND::IMPORT)) {
        if (msg->isSuccess()) {
            QMessageBox::information(this, tr("Import success"), tr("Import success. Application need to restart."));
            qApp->quit();
            QStringList list;
            const QStringList &args = qApp->arguments();
            for (int i = 0; i < args.count(); i++) {
                if (i == 0)
                    continue;
                list.append(args[i]);
            }
            QProcess::startDetached(qApp->arguments()[0], list);
        } else {
            QMessageBox::critical(this, tr("Error"), msg->data("error").toString());
        }
        GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                     ui->pushImportGDrive});
    }
}

bool ImportExportDatabaseDialog::eventFilter(QObject *obj, QEvent *event) {
    auto webview = qobject_cast<BrowserDialog *>(obj);
    if (webview != nullptr) {
        if (event->type() == QEvent::Close)
            GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive,
                                                         ui->pushImportFile, ui->pushImportGDrive});
    }
    return QDialog::eventFilter(obj, event);
}

void ImportExportDatabaseDialog::uploadFile(const QByteArray &data) {
    Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::IMPORT);
    const QByteArray &ba = data.toBase64();
    msg.addData("data", QString(ba));
    sendMessage(&msg);
}

void ImportExportDatabaseDialog::exportFile() {
    Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::EXPORT);
    sendMessage(&msg);
    GuiUtil::enableWidget(false, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                  ui->pushImportGDrive});
}

void ImportExportDatabaseDialog::importFile() {
    const QString &filename = QFileDialog::getOpenFileName(this, tr("Import database"), QDir::homePath(), "*.sltn");
    if (!filename.isEmpty()) {
        GuiUtil::enableWidget(false, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                      ui->pushImportGDrive});

        QFile f(filename);
        if (f.open(QFile::ReadOnly)) {
            uploadFile(f.readAll());
        }
    }
}
