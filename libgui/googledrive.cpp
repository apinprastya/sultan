/*
 * googledrive.cpp
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
#include "googledrive.h"
#include "o2reply.h"
#include "o2requestor.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace LibGUI;

static QString G_FILES{"https://www.googleapis.com/drive/v3/files?q=mimeType=\"application/octet-stream\" and "
                       "trashed=false and \"%1\" in parents&orderBy=createdTime desc"};
static QString G_GET_FOLDER{"https://www.googleapis.com/drive/v3/files?q=mimeType=\"application/"
                            "vnd.google-apps.folder\" and name=\"sultanbackup\" and trashed=false"};
static QString G_UPLOAD{"https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart"};
static QString G_DOWNLOAD{"https://www.googleapis.com/drive/v3/files/%1?alt=media"};

GoogleDrive::GoogleDrive(O2Google *googleAuth, QObject *parent)
    : QObject(parent), mAuth(googleAuth), mManager(new QNetworkAccessManager(this)) {}

void GoogleDrive::getFolder() {
    QNetworkRequest req;
    req.setUrl(G_GET_FOLDER);
    auto request = new O2Requestor(mManager, mAuth, this);
    connect(request, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
            SLOT(getFolderDone(int, QNetworkReply::NetworkError, QByteArray)));
    request->get(req);
}

void GoogleDrive::getFiles() {
    mIsGetFiles = true;
    getFolder();
}

void GoogleDrive::createFolder() {
    QNetworkRequest req;
    req.setUrl(G_FILES);
    QByteArray jsonString = "{\"name\":\"sultanbackup\",\"mimeType\":\"application/vnd.google-apps.folder\"}";
    QByteArray size = QByteArray::number(jsonString.size());
    req.setRawHeader("Content-Type", "application/json");
    req.setRawHeader("Content-Length", size);
    auto request = new O2Requestor(mManager, mAuth, this);
    connect(request, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
            SLOT(createFolderDone(int, QNetworkReply::NetworkError, QByteArray)));
    request->post(req, jsonString);
}

void GoogleDrive::uploadFileExec() {
    QByteArray data;
    data.reserve(2048000); // 2MB
    data.append("--sultan_boundary\nContent-Type: application/json; charset=UTF-8\n\n{ \"name\": \"");
    data.append("sultan" + QDateTime::currentDateTime().toString("-yyyyMMdd-hhmmss") + "\", \"parents\": [\"");
    data.append(mFolderId + "\"]}\n\n");
    data.append("--sultan_boundary\nContent-Type: application/octet-stream\n\n");
    data.append(mDataToUpload);
    data.append("\n\n--sultan_boundary--");
    QNetworkRequest req;
    req.setUrl(G_UPLOAD);
    req.setRawHeader("Content-Type", "multipart/related; boundary=sultan_boundary");
    req.setRawHeader("Content-Length", QByteArray::number(data.size()));
    auto request = new O2Requestor(mManager, mAuth, this);
    connect(request, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
            SLOT(uploadFileDone(int, QNetworkReply::NetworkError, QByteArray)));
    connect(request, SIGNAL(uploadProgress(int, qint64, qint64)), SLOT(onUploadProgress(int, qint64, qint64)));
    request->post(req, data);
}

void GoogleDrive::getFileExec() {
    QNetworkRequest req;
    req.setUrl(G_FILES.arg(mFolderId));
    auto request = new O2Requestor(mManager, mAuth, this);
    connect(request, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
            SLOT(getFileDone(int, QNetworkReply::NetworkError, QByteArray)));
    request->get(req);
}

void GoogleDrive::uploadFile(const QByteArray &data) {
    mIsGetFiles = false;
    mDataToUpload = data;
    getFolder();
}

void GoogleDrive::downloadFile(const QString &fileid) {
    QNetworkRequest req;
    req.setUrl(G_DOWNLOAD.arg(fileid));
    auto request = new O2Requestor(mManager, mAuth, this);
    connect(request, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
            SLOT(downloadDone(int, QNetworkReply::NetworkError, QByteArray)));
    connect(request, SIGNAL(downloadProgress(int, qint64, qint64)), SLOT(onDownloadProgress(int, qint64, qint64)));
    request->get(req);
}

void GoogleDrive::getFolderDone(int /*id*/, QNetworkReply::NetworkError /*error*/, QByteArray data) {
    auto json = QJsonDocument::fromJson(data).object();
    auto folders = json.value("files").toArray();
    if (folders.size() == 0) {
        createFolder();
    } else {
        auto j = folders[0].toObject();
        mFolderId = j.value("id").toString();
        // qDebug() << "[GOOGLE] Folder id : " << mFolderId;
        if (mIsGetFiles)
            getFileExec();
        else
            uploadFileExec();
    }
}

void GoogleDrive::getFileDone(int /*id*/, QNetworkReply::NetworkError /*error*/, QByteArray data) {
    auto json = QJsonDocument::fromJson(data).object();
    auto arr = json.value("files").toArray();
    emit fileQueryAnswered(arr);
}

void GoogleDrive::createFolderDone(int /*id*/, QNetworkReply::NetworkError /*error*/, QByteArray data) {
    auto json = QJsonDocument::fromJson(data).object();
    mFolderId = json.value("id").toString();
    if (mIsGetFiles)
        getFileExec();
    else
        uploadFileExec();
}

void GoogleDrive::uploadFileDone(int /*id*/, QNetworkReply::NetworkError /*error*/, QByteArray /*data*/) {
    emit fileUploaded();
}

void GoogleDrive::onUploadProgress(int /*id*/, qint64 uploaded, qint64 total) {
    if (total == 0) {
        emit uploadProgress(0);
        return;
    }
    int val = (int)uploaded * 100 / (int)total;
    emit uploadProgress(val);
}

void GoogleDrive::onDownloadProgress(int /*id*/, qint64 downloaded, qint64 total) {
    if (total == 0) {
        emit downloadProgress(0);
    } else {
        emit downloadProgress((int)downloaded * 100 / (int)total);
    }
}

void GoogleDrive::downloadDone(int /*id*/, QNetworkReply::NetworkError /*error*/, QByteArray data) {
    auto req = qobject_cast<O2Requestor *>(QObject::sender());
    auto reply = req->getReply();
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 300 && status < 400) {
        const QString &url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        QNetworkRequest req;
        req.setUrl(url);
        auto reply = mManager->get(req);
        connect(reply, SIGNAL(finished()), SLOT(downloadDone()));
        connect(reply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
    } else {
        emit fileDownloaded(data);
    }
}

void GoogleDrive::onDownloadProgress(qint64 downloaded, qint64 total) {
    if (total == 0) {
        emit downloadProgress(0);
    } else {
        emit downloadProgress((int)downloaded * 100 / (int)total);
    }
}

void GoogleDrive::downloadDone() {
    auto reply = qobject_cast<QNetworkReply *>(QObject::sender());
    const QByteArray &data = reply->readAll();
    emit fileDownloaded(data);
}
