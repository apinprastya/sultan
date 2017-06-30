/*
 * autoupdatedialog.cpp
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
#include "autoupdatedialog.h"
#include "ui_autoupdatedialog.h"
#include "global_constant.h"
#include "util.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>

using namespace LibGUI;

AutoUpdateDialog::AutoUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoUpdateDialog),
    mNetworkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    mGroupWidget << ui->line << ui->line_2 << ui->labelNV << ui->labelCV << ui->labelCurrentVersion << ui->labelNewVersion <<
                    ui->pushUpdate << ui->progressBar;
    for(auto widget : mGroupWidget)
        widget->hide();
    adjustSize();
    if(LibG::Util::isBetaVersion(qApp->applicationVersion())) {
        ui->labelChecking->setText(tr("Beta version has no auto update"));
    } else {
#ifdef Q_OS_WIN32
        mArc = "win32";
#elif defined (Q_OS_LINUX)
        mArc = "linux";
#elif defined (Q_PROCESSOR_ARM)
        mArc = "raspberry";
#endif
        QNetworkRequest request(QUrl(LibG::CONSTANT::URL_UPDATE.arg(mArc).arg(qVersion())));
        auto reply = mNetworkManager->get(request);
        connect(reply, SIGNAL(finished()), SLOT(checkDone()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(httpError(QNetworkReply::NetworkError)));
    }
    connect(ui->pushUpdate, SIGNAL(clicked(bool)), SLOT(updateClicked()));
}

AutoUpdateDialog::~AutoUpdateDialog()
{
    delete ui;
}

void AutoUpdateDialog::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
}

void AutoUpdateDialog::checkDone()
{
    auto reply = static_cast<QNetworkReply*>(QObject::sender());
    mNewVersion = QString::fromUtf8(reply->readAll());
    int version = LibG::Util::getIntVersion(mNewVersion);
    int curVersion = LibG::Util::getIntVersion(qApp->applicationVersion());
    if(version > curVersion) {
        for(auto widget : mGroupWidget)
            widget->show();
    }
    adjustSize();
}

void AutoUpdateDialog::httpError(QNetworkReply::NetworkError /*error*/)
{
    auto reply = static_cast<QNetworkReply*>(QObject::sender());
    ui->labelChecking->setText(reply->errorString());
}

void AutoUpdateDialog::updateClicked()
{
    QNetworkRequest request(QUrl(LibG::CONSTANT::URL_DOWNLOAD.arg(mNewVersion).arg(mArc).arg(qVersion())));
    auto reply = mNetworkManager->get(request);
    connect(reply, SIGNAL(finished()), SLOT(downloadDone()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(httpError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
}

void AutoUpdateDialog::downloadDone()
{
    auto reply = static_cast<QNetworkReply*>(QObject::sender());
#ifdef Q_OS_WIN32
    QDir dir(qApp->applicationDirPath());
    dir.mkdir("update");
    dir.cd("update");
#else
    QDir dir = QDir::home();
    dir.cd(".sultan");
#endif
    QFile file(dir.absoluteFilePath("sultan.zip"));
    if(file.exists()) file.remove();
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), file.errorString());
        return;
    }
    file.write(reply->readAll());
    file.close();
    int ret = QMessageBox::question(this, tr("Downloaded"), tr("New update downloaded successfully. Retart application to update?"));
    if(ret == QMessageBox::Yes) {
        QDir dir(qApp->applicationDirPath());
#ifdef Q_OS_WIN32
        QProcess::startDetached(dir.absoluteFilePath("zipextractor.exe"));
#else
        QProcess::startDetached(dir.absoluteFilePath("zipextractor"));
#endif
        qApp->quit();
    }
}

void AutoUpdateDialog::downloadProgress(qint64 receive, qint64 total)
{
    ui->progressBar->setValue((int)(receive * 100 / total));
}
