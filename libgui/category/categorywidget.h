/*
 * categorywidget.h
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
#ifndef CATEGORYWIDGET_H
#define CATEGORYWIDGET_H

#include "messagehandler.h"
#include <QWidget>

class QPushButton;
class QHBoxLayout;

namespace Ui {
class NormalWidget;
}

namespace LibGUI {

class CategoryTreeWidget;
class CategoryAddDialog;

class CategoryWidget : public QWidget, public LibG::MessageHandler
{
    Q_OBJECT
public:
    CategoryWidget(LibG::MessageBus *bus, QWidget *parent = nullptr);
    ~CategoryWidget();

protected:
    void messageReceived(LibG::Message *msg);

private:
    Ui::NormalWidget *ui;
    CategoryTreeWidget *mTreeWidget;
    CategoryAddDialog *mAddDialog;
    int mDeletedId;
    QPushButton *addButtonAction(QHBoxLayout *layout, const QString &iconName);
    void pupulateComboBox();

private slots:
    void loadCategory();
    void addClicked();
    void updateClicked();
    void deleteClicked();
    void saveRequested(const QVariantMap &data, int id);
};

}
#endif // CATEGORYWIDGET_H
