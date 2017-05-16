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

#include <QWidget>

class QComboBox;

namespace Ui {
class SettingWidget;
}

namespace LibGUI {

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = 0);
    ~SettingWidget();

private:
    Ui::SettingWidget *ui;
    void setupAppliaction();
    void setupLocale();
    void setupPrinter();
    void setCurrentCombo(QComboBox *combo, QVariant value);

private slots:
    void signChanged();
    void saveClicked();
};

}
#endif // SETTINGWIDGET_H
