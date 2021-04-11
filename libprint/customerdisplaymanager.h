/*
 * Copyright 2021, Apin <apin.klas@gmail.com>
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

#ifndef CUSTOMERDISPLAYMANAGER_H
#define CUSTOMERDISPLAYMANAGER_H

#include "print_global.h"
#include <QObject>
#include <QSerialPort>

namespace LibPrint {

class PRINTSHARED_EXPORT CustomerDisplayManager : public QObject {
    Q_OBJECT
  public:
    static void createInstance(QObject *parent);
    static CustomerDisplayManager *instance();
    ~CustomerDisplayManager();
    bool open();
    bool openPort(int vendorId, int productId);
    void close();
    bool write(const QByteArray &data);
    void wait();

  private:
    CustomerDisplayManager(QObject *parent = nullptr);
    QSerialPort mPort;
    int mVendorId;
    int mProductId;

  private slots:
    void bytesWritten();
};

} // namespace LibPrint

#endif // CUSTOMERDISPLAYMANAGER_H
