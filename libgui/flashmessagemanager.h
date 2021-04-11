/*
 * flashmessagemanager.h
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
#ifndef FLASHMESSAGEMANAGER_H
#define FLASHMESSAGEMANAGER_H

#include "flashmessage.h"
#include "gui_global.h"
#include <QObject>

namespace LibGUI {

class GUISHARED_EXPORT FlashMessageManager : public QObject {
    Q_OBJECT
  public:
    static void showMessage(const QString &message, int type = FlashMessage::Info, int lifetime = FlashMessage::Short);
    static void showError(const QString &message, int lifetime = FlashMessage::Short);
    static void showWarning(const QString &message, int lifetime = FlashMessage::Short);
    static void setParent(QWidget *parent);

  private:
    FlashMessageManager(QObject *parent = 0);

    QList<FlashMessage *> mFlashMessages;
    QWidget *mParent = nullptr;

  private slots:
    void flashMessageClosed(FlashMessage *flashmessage);

  private:
    void rearrangeMessage();
};

} // namespace LibGUI
#endif // FLASHMESSAGEMANAGER_H
