/*
 * flashmessage.h
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
#ifndef FLASHMESSAGE_H
#define FLASHMESSAGE_H

#include "gui_global.h"
#include <QWidget>

class QTimer;

namespace LibGUI {

class GUISHARED_EXPORT FlashMessage : public QWidget {
    Q_OBJECT
  public:
    enum Type { Info, Warning, Error };
    enum Lifetime { Short, Long };
    FlashMessage(const QString &message, int type = Info, int lifetime = Short, QWidget *parent = nullptr);
    ~FlashMessage();
    void mouseReleaseEvent(QMouseEvent *event) override;

  signals:
    void done(FlashMessage *flashmessage);

  private:
    QTimer *mTimer;

  private slots:
    void lifetimeDone();
};

} // namespace LibGUI
#endif // FLASHMESSAGE_H
