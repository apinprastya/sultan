/*
 * futurewatcher.h
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
#ifndef FUTUREWATCHER_H
#define FUTUREWATCHER_H

#include "global_global.h"
#include "message.h"
#include <QFutureWatcher>
#include <QObject>

namespace LibG {

class GLOBALSHARED_EXPORT FutureWatcher : public QObject {
    Q_OBJECT
  public:
    FutureWatcher(QObject *parent = 0);
    void setFuture(const QFuture<Message> &future);

  private:
    QFutureWatcher<Message> *mWatcher;

  private slots:
    void finished();

  signals:
    void messageReceived(LibG::Message *msg);
};

} // namespace LibG
#endif // FUTUREWATCHER_H
