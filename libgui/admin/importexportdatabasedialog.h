/*
 * importexportdatabasedialog.h
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
#ifndef IMPORTEXPORTDATABASEDIALOG_H
#define IMPORTEXPORTDATABASEDIALOG_H

#include "messagehandler.h"
#include "o2google.h"
#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class ImportExportDatabaseDialog;
}

class QProgressDialog;

namespace LibGUI {

class BrowserDialog;
class GoogleDrive;

class ImportExportDatabaseDialog : public QDialog, public LibG::MessageHandler {
    Q_OBJECT

  public:
    ImportExportDatabaseDialog(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~ImportExportDatabaseDialog() override;

  protected:
    void messageReceived(LibG::Message *msg) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

  private:
    Ui::ImportExportDatabaseDialog *ui;
    bool mIsGDrive = false;
    BrowserDialog *mBrowser;
    O2Google *mO2Google;
    GoogleDrive *mGDrive;
    QProgressDialog *mProgressDialog = nullptr;
    QNetworkAccessManager mNetworkManager;
    bool mIsExport = false;
    bool mGDriveInProcess = false;

    void uploadFile(const QByteArray &data);

  private slots:
    void exportFile();
    void exportGDrive();
    void importFile();
    void importGDrive();
    void onLinkingFailed();
    void onLinkingSuccess();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();
    void uploadGDriveDone();
    void onFileListed(const QJsonArray &arr);
    void onFileDownloaded(const QByteArray &data);
    void requestFinished(QNetworkReply *reply);
};

} // namespace LibGUI
#endif // IMPORTEXPORTDATABASEDIALOG_H
