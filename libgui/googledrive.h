/*
 * googledrive.h
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
#ifndef GOOGLEDRIVE_H
#define GOOGLEDRIVE_H

#include "o2google.h"
#include <QObject>

namespace LibGUI {

class GoogleDrive : public QObject {
    Q_OBJECT
  public:
    GoogleDrive(O2Google *googleAuth, QObject *parent = 0);
    void getFiles();
    void uploadFile(const QByteArray &data);
    void downloadFile(const QString &fileid);

  private:
    O2Google *mAuth;
    QNetworkAccessManager *mManager;
    QString mFolderId;
    QByteArray mDataToUpload;
    bool mIsGetFiles = false;

    void getFolder();
    void createFolder();
    void uploadFileExec();
    void getFileExec();

  signals:
    void fileQueryAnswered(const QJsonArray &list);
    void fileUploaded();
    void uploadProgress(int value);
    void downloadProgress(int value);
    void fileDownloaded(const QByteArray &data);

  private slots:
    void getFolderDone(int id, QNetworkReply::NetworkError error, QByteArray data);
    void getFileDone(int id, QNetworkReply::NetworkError error, QByteArray data);
    void createFolderDone(int id, QNetworkReply::NetworkError error, QByteArray data);
    void uploadFileDone(int id, QNetworkReply::NetworkError error, QByteArray data);
    void onUploadProgress(int id, qint64 uploaded, qint64 total);
    void onDownloadProgress(int id, qint64 downloaded, qint64 total);
    void downloadDone(int id, QNetworkReply::NetworkError error, QByteArray data);
    void onDownloadProgress(qint64 downloaded, qint64 total);
    void downloadDone();
};

} // namespace LibGUI
#endif // GOOGLEDRIVE_H
