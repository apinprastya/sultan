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
#include "googledrive.h"
#include "guiutil.h"
#include "listdialog.h"
#include "message.h"
#include "preference.h"
#include "ui_importexportdatabasedialog.h"
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QProgressDialog>

using namespace LibGUI;
using namespace LibG;

ImportExportDatabaseDialog::ImportExportDatabaseDialog(LibG::MessageBus *bus, QWidget *parent)
    : QDialog(parent), ui(new Ui::ImportExportDatabaseDialog), mBrowser(new BrowserDialog(this)),
      mO2Google(new O2Google(this)) {
    ui->setupUi(this);
    setMessageBus(bus);
    mBrowser->installEventFilter(this);
    mO2Google->setClientId(GOOGLE::CLIENT_ID);
    mO2Google->setClientSecret(GOOGLE::CLIENT_SECRET);
    mO2Google->setScope("https://www.googleapis.com/auth/drive");
    mGDrive = new GoogleDrive(mO2Google, this);
    adjustSize();
    connect(ui->pushExportToFile, SIGNAL(clicked(bool)), SLOT(exportFile()));
    connect(ui->pushExportToGDrive, SIGNAL(clicked(bool)), SLOT(exportGDrive()));
    connect(ui->pushImportFile, SIGNAL(clicked(bool)), SLOT(importFile()));
    connect(ui->pushImportGDrive, SIGNAL(clicked(bool)), SLOT(importGDrive()));
    connect(mO2Google, SIGNAL(linkingFailed()), SLOT(onLinkingFailed()));
    connect(mO2Google, SIGNAL(linkingSucceeded()), SLOT(onLinkingSuccess()));
    connect(mO2Google, SIGNAL(openBrowser(QUrl)), SLOT(onOpenBrowser(const QUrl)));
    connect(mO2Google, SIGNAL(closeBrowser()), SLOT(onCloseBrowser()));
    connect(mGDrive, SIGNAL(fileUploaded()), SLOT(uploadGDriveDone()));
    connect(mGDrive, SIGNAL(fileQueryAnswered(QJsonArray)), SLOT(onFileListed(QJsonArray)));
    connect(mGDrive, SIGNAL(fileDownloaded(QByteArray)), SLOT(onFileDownloaded(QByteArray)));
    connect(&mNetworkManager, SIGNAL(finished(QNetworkReply *)), SLOT(requestFinished(QNetworkReply *)));
}

ImportExportDatabaseDialog::~ImportExportDatabaseDialog() { delete ui; }

void ImportExportDatabaseDialog::messageReceived(Message *msg) {
    if (msg->isTypeCommand(MSG_TYPE::DATABASE, MSG_COMMAND::EXPORT)) {
        if (msg->isSuccess()) {
            QString url;
            if (Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
                url = "http://localhost:" + QString::number(Preference::getInt(SETTING::APP_PORT) + 1);
            } else {
                url = "http://" + Preference::getString(SETTING::SERVER_ADDRESS) + ":" +
                      QString::number(Preference::getInt(SETTING::SERVER_PORT) + 1);
            }
            url += "/sultan.export";
            QUrl qurl(url);
            QNetworkRequest req(qurl);
            mNetworkManager.get(req);
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
        if (event->type() == QEvent::Close && !mGDriveInProcess)
            GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive,
                                                         ui->pushImportFile, ui->pushImportGDrive});
    }
    return QDialog::eventFilter(obj, event);
}

void ImportExportDatabaseDialog::uploadFile(const QByteArray &data) {
    QString url;
    if (Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER) {
        url = "http://localhost:" + QString::number(Preference::getInt(SETTING::APP_PORT) + 1);
    } else {
        url = "http://" + Preference::getString(SETTING::SERVER_ADDRESS) + ":" +
              QString::number(Preference::getInt(SETTING::SERVER_PORT) + 1);
    }
    QUrl qurl(url);
    QNetworkRequest req(qurl);
    auto rep = mNetworkManager.post(req, data);
    rep->setObjectName("UPLOAD");
}

void ImportExportDatabaseDialog::exportFile() {
    mIsGDrive = false;
    Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::EXPORT);
    msg.addData("version", qApp->applicationVersion());
    sendMessage(&msg);
    GuiUtil::enableWidget(false, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                  ui->pushImportGDrive});
}

void ImportExportDatabaseDialog::exportGDrive() {
    mGDriveInProcess = false;
    mIsGDrive = true;
    mIsExport = true;
    mO2Google->link();
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

void ImportExportDatabaseDialog::importGDrive() {
    mGDriveInProcess = false;
    mIsGDrive = true;
    mIsExport = false;
    mO2Google->link();
    GuiUtil::enableWidget(false, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                  ui->pushImportGDrive});
}

void ImportExportDatabaseDialog::onLinkingFailed() {
    QMessageBox::warning(this, tr("Google Auth"), tr("Google Auth Failed"));
    mGDriveInProcess = false;
}

void ImportExportDatabaseDialog::onLinkingSuccess() {
    if (!mO2Google->linked())
        return;
    mGDriveInProcess = true;
    if (mIsExport) {
        Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::EXPORT);
        msg.addData("version", qApp->applicationVersion());
        sendMessage(&msg);
    } else {
        mGDrive->getFiles();
    }
}

void ImportExportDatabaseDialog::onOpenBrowser(const QUrl &url) {
#ifdef USE_EMBED_BROWSER
    mBrowser->setUrl(url);
    mBrowser->show();
#else
    QDesktopServices::openUrl(url);
#endif
}

void ImportExportDatabaseDialog::onCloseBrowser() {
    mGDriveInProcess = false;
#ifdef USE_EMBED_BROWSER
    mBrowser->hide();
#else
#endif
}

void ImportExportDatabaseDialog::uploadGDriveDone() {
    GuiUtil::enableWidget(
        true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile, ui->pushImportGDrive});
}

void ImportExportDatabaseDialog::onFileListed(const QJsonArray &arr) {
    ListDialog dialog(this);
    dialog.fill(arr);
    dialog.exec();
    const QString fileId = dialog.getSelectedId();
    if (!fileId.isEmpty()) {
        mGDrive->downloadFile(fileId);
    } else {
        GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive, ui->pushImportFile,
                                                     ui->pushImportGDrive});
    }
}

void ImportExportDatabaseDialog::onFileDownloaded(const QByteArray &data) { uploadFile(data); }

void ImportExportDatabaseDialog::requestFinished(QNetworkReply *reply) {
    const QByteArray &data = reply->readAll();
    if (!mIsGDrive) {
        if (reply->objectName() == "UPLOAD") {
            Message msg(MSG_TYPE::DATABASE, MSG_COMMAND::IMPORT);
            msg.addData("version", qApp->applicationVersion());
            msg.addData("name", QString::fromUtf8(data));
            sendMessage(&msg);
        } else {
            auto filename =
                QFileDialog::getSaveFileName(this, tr("Save exported database"), QDir::homePath(), "*.sltn");
            if (!filename.isEmpty()) {
                if (!filename.endsWith(".sltn"))
                    filename += ".sltn";
                QFile file(filename);
                if (file.open(QFile::WriteOnly)) {
                    file.write(data);
                    file.close();
                }
            }
            GuiUtil::enableWidget(true, QList<QWidget *>{ui->pushExportToFile, ui->pushExportToGDrive,
                                                         ui->pushImportFile, ui->pushImportGDrive});
        }
    } else {
        if (reply->objectName() == "UPLOAD") {
        } else {
            mGDrive->uploadFile(data);
        }
    }
}
