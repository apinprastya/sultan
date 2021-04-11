/*
 * settingdialog.cpp
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
#include "settingdialog.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "keyevent.h"
#include "preference.h"
#include "ui_settingdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlError>

using namespace LibGUI;
using namespace LibG;

static std::function<void(const QString &, int)> sOpenCon = nullptr;
static std::function<void()> sCloseCon = nullptr;
static std::function<void()> sConConnected = nullptr;
static std::function<void(const QString &)> sConError = nullptr;
static std::function<void()> sConTimeout = nullptr;

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingDialog), mMysqlOk(false), mConOk(false) {
    ui->setupUi(this);
    ui->pushSave->setDisabled(true);
    ui->comboType->addItem(tr("Server"), APPLICATION_TYPE::SERVER);
    ui->comboType->addItem(tr("Client"), APPLICATION_TYPE::CLIENT);
    ui->comboDatabase->addItem("SQLITE");
    ui->comboDatabase->addItem("MYSQL");

    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), SLOT(checkType()));
    connect(ui->pushCheckMysql, SIGNAL(clicked(bool)), SLOT(checkMysql()));
    connect(ui->pushCheckConnection, SIGNAL(clicked(bool)), SLOT(checkConnection()));
    connect(ui->pushCancel, SIGNAL(clicked(bool)), SLOT(cancel()));
    connect(ui->pushSave, SIGNAL(clicked(bool)), SLOT(save()));
    connect(ui->comboDatabase, SIGNAL(currentIndexChanged(int)), SLOT(databaseChanged()));
    QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), QStringLiteral("settingtest"));

    checkType();
    databaseChanged();
    sConConnected = std::bind(&SettingDialog::clientConnected, this);
    sConError = std::bind(&SettingDialog::clientError, this, std::placeholders::_1);
    sConTimeout = std::bind(&SettingDialog::clientTimeout, this);
    auto ke = new KeyEvent(this);
    ke->setClickEvent(true);
    ui->lineSqlitePath->installEventFilter(ke);
    connect(ke, SIGNAL(clicked(QObject *)), SLOT(openDirectorySelector()));
    ui->lineSqlitePath->setReadOnly(true);
}

SettingDialog::~SettingDialog() {
    delete ui;
    sConConnected = nullptr;
    sConError = nullptr;
    sConTimeout = nullptr;
}

void SettingDialog::showDialog() {
    ui->comboType->setCurrentIndex(Preference::getInt(SETTING::APP_TYPE) == APPLICATION_TYPE::SERVER ? 0 : 1);
    ui->comboDatabase->setCurrentIndex(Preference::getString(SETTING::DATABASE) == "MYSQL" ? 1 : 0);
    ui->lineSqlitePath->setText(Preference::getString(SETTING::SQLITE_DBPATH));
    ui->lineSqliteName->setText(Preference::getString(SETTING::SQLITE_DBNAME));
    ui->lineEditHost->setText(Preference::getString(SETTING::MYSQL_HOST));
    ui->spinBoxPort->setValue(Preference::getInt(SETTING::MYSQL_PORT, 3306));
    ui->lineEditUsername->setText(Preference::getString(SETTING::MYSQL_USERNAME));
    ui->lineEditPassword->setText(Preference::getString(SETTING::MYSQL_PASSWORD));
    ui->lineEditDatabase->setText(Preference::getString(SETTING::MYSQL_DB));
    show();
}

void SettingDialog::setSettingSocketOpenClose(std::function<void(const QString &, int)> openCon,
                                              std::function<void()> closeCon) {
    sOpenCon = openCon;
    sCloseCon = closeCon;
}

void SettingDialog::guiMessage(int id, const QString &str) {
    if (id == GUI_MESSAGE::MSG_CONNECTION_SUCCESS)
        sConConnected();
    else if (id == GUI_MESSAGE::MSG_CONNECTION_FAILED)
        sConError(str);
    else if (id == GUI_MESSAGE::MSG_CONNECTION_TIMEOUT)
        sConTimeout();
}

void SettingDialog::saveMysqlSetting() {
    Preference::setValue(SETTING::MYSQL_HOST, ui->lineEditHost->text());
    Preference::setValue(SETTING::MYSQL_PORT, ui->spinBoxPort->value());
    Preference::setValue(SETTING::MYSQL_USERNAME, ui->lineEditUsername->text());
    Preference::setValue(SETTING::MYSQL_PASSWORD, ui->lineEditPassword->text());
    Preference::setValue(SETTING::MYSQL_DB, ui->lineEditDatabase->text());
    Preference::sync();
}

void SettingDialog::databaseChanged() {
    bool isMysql = ui->comboDatabase->currentText() == "MYSQL";
    ui->groupMysql_2->setEnabled(isMysql);
    ui->groupSqlite->setEnabled(!isMysql);
    mMysqlOk = !isMysql;
    checkSetting();
}

void SettingDialog::checkSetting() {
    int type = ui->comboType->currentData().toInt();
    if (type == APPLICATION_TYPE::SERVER) {
        ui->pushSave->setEnabled(mMysqlOk);
    } else {
        ui->pushSave->setEnabled(mConOk);
    }
}

void SettingDialog::checkType() {
    int type = ui->comboType->currentData().toInt();
    ui->groupClient->setEnabled(type != APPLICATION_TYPE::SERVER);
    ui->groupMysql->setEnabled(type == APPLICATION_TYPE::SERVER);
    ui->groupServer->setEnabled(type == APPLICATION_TYPE::SERVER);
    checkSetting();
}

void SettingDialog::checkMysql() {
    if (ui->lineEditHost->text().isEmpty() || ui->lineEditUsername->text().isEmpty() ||
        ui->lineEditDatabase->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please complete the setting"));
        return;
    }
    QSqlDatabase database = QSqlDatabase::database(QStringLiteral("settingtest"));
    database.setHostName(ui->lineEditHost->text());
    database.setPort(ui->spinBoxPort->value());
    // database.setDatabaseName(ui->lineEditDatabase->text());
    database.setUserName(ui->lineEditUsername->text());
    database.setPassword(ui->lineEditPassword->text());
    if (database.open()) {
        QMessageBox::information(this, tr("Success"), tr("Connection to Mysql OK!"));
        Preference::setValue(SETTING::MYSQL_OK, true);
        mMysqlOk = true;
        saveMysqlSetting();
    } else {
        QMessageBox::critical(this, tr("Error"), database.lastError().text());
        Preference::setValue(SETTING::MYSQL_OK, false);
        mMysqlOk = false;
    }
    checkSetting();
}

void SettingDialog::checkConnection() {
    if (sOpenCon != nullptr)
        sOpenCon(ui->lineEditClientAddress->text(), ui->spinBoxClientPort->value());
}

void SettingDialog::cancel() {
    if (!Preference::getBool(SETTING::SETTING_OK)) {
        close();
        qApp->quit();
    } else {
        hide();
    }
}

void SettingDialog::save() {
    Preference::setValue(SETTING::APP_TYPE, ui->comboType->currentData().toInt());
    Preference::setValue(SETTING::APP_PORT, ui->spinBoxServerPort->text());
    Preference::setValue(SETTING::SERVER_PORT, ui->spinBoxClientPort->value());
    Preference::setValue(SETTING::SERVER_ADDRESS, ui->lineEditClientAddress->text());
    Preference::setValue(SETTING::DATABASE, ui->comboDatabase->currentText());
    Preference::setValue(SETTING::SQLITE_DBPATH, ui->lineSqlitePath->text());
    Preference::setValue(SETTING::SQLITE_DBNAME, ui->lineSqliteName->text());
    Preference::setValue(SETTING::SETTING_OK, true);
    Preference::sync();
    // restart the app for easier :D
    qApp->quit();
    QStringList list;
    const QStringList &args = qApp->arguments();
    for (int i = 0; i < args.count(); i++) {
        if (i == 0)
            continue;
        list.append(args[i]);
    }
    QProcess::startDetached(qApp->arguments()[0], list);
    if (sCloseCon != nullptr)
        sCloseCon();
}

void SettingDialog::openDirectorySelector() {
    auto str = QFileDialog::getExistingDirectory(this, tr("Select directory"), QDir::homePath());
    if (!str.isEmpty()) {
        ui->lineSqlitePath->setText(str);
    }
}

void SettingDialog::clientConnected() {
    QMessageBox::information(this, tr("Connection Success"), tr("Connection to server success"));
    mConOk = true;
    checkSetting();
}

void SettingDialog::clientError(const QString &err) {
    QMessageBox::critical(this, tr("Connection Error"), tr("Connection error : %1").arg(err));
}

void SettingDialog::clientTimeout() { QMessageBox::critical(this, tr("Connection Timeout"), tr("Connection timeout")); }
