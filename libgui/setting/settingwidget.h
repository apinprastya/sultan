/*
 * settingwidget.h
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
#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include "messagehandler.h"
#include "usb.h"
#include <QLocale>
#include <QMap>
#include <QWidget>

class QComboBox;

namespace Ui {
class SettingWidget;
}

namespace LibGUI {

class SettingWidget : public QWidget, public LibG::MessageHandler {
    Q_OBJECT

  public:
    SettingWidget(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~SettingWidget();

  private:
    Ui::SettingWidget *ui;
    QMap<QString, QLocale::Language> mLocaleLanguage;
    QMap<QString, QLocale::Country> mLocaleCountry;
    QList<QLocale> mAllLocales;
    QList<UsbDevice> mUsbDevices;
    void setupAppliaction();
    void setupLocale();
    void setupPrinter();
    void setupCashier();
    void setCurrentCombo(QComboBox *combo, QVariant value);
    void updateFromDBConfig(const QVariantList &data);
    void saveToDbConfig();
    void setupCustomerDisplay();

  private slots:
    void signChanged();
    void cashierPrintTypeChanged();
    void saveClicked();
    void tabChanged();
    void printTestClicked();
    void localeLanguageChanged();
    void checkWidget();
    void onCustomerDisplayClicked();

  protected:
    void messageReceived(LibG::Message *msg) override;
};

} // namespace LibGUI
#endif // SETTINGWIDGET_H
