/*
 * messagebus.h
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
#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include "global_global.h"
#include <QList>
#include <QObject>

namespace LibG {

class Message;
class MessageHandler;

class GLOBALSHARED_EXPORT MessageBus : public QObject {
    Q_OBJECT
  public:
    MessageBus(QObject *parent = nullptr);
    void sendMessage(Message *msg);
    void registerHandler(MessageHandler *handler);
    void removeHandler(MessageHandler *handler);

  public slots:
    void messageRecieved(LibG::Message *msg);

  signals:
    void newMessageToSend(LibG::Message *msg);

  private:
    QList<MessageHandler *> mMessageHandler;
};

} // namespace LibG
#endif // MESSAGEBUS_H
