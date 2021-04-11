/*
 * printer.cpp
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
#include "printer.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include "preference.h"
#ifdef USE_LIBUSB
#include "usb.h"
#endif
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QPrinterInfo>
#ifdef Q_OS_WIN
#include <windows.h>
#else
#ifndef NO_PRINTER_SPOOL
#include <cups/cups.h>
#endif
#endif

using namespace LibPrint;
using namespace LibG;

static Printer *sInstance = Q_NULLPTR;

Printer::Printer() {}

Printer *Printer::instance() {
    if (!sInstance)
        sInstance = new Printer();
    return sInstance;
}

void Printer::destroy() {
    if (sInstance)
        delete sInstance;
    sInstance = Q_NULLPTR;
}

QStringList Printer::getPrintList() { return QPrinterInfo::availablePrinterNames(); }

// this method is designed to be called from separated thread
void Printer::print(const QString &printName, const QString &data, int type, uint16_t vendorId, uint16_t produckId) {
    QMutexLocker locker(&mMutex);
#ifdef Q_OS_WIN
    wchar_t printerName[256];
    printName.toWCharArray(printerName);
    printerName[printName.length()] = '\0';
    LPBYTE lpData;
    QByteArray ba = data.toLocal8Bit();
    lpData = (unsigned char *)(ba.data());
    DWORD dwCount = ba.length();

    DOC_INFO_1 docInfo;
    wchar_t docName[16], dataType[8];
    wcscpy_s(docName, 100, L"Sultan Document");
    wcscpy_s(dataType, 100, L"RAW");
    docInfo.pOutputFile = NULL;
    docInfo.pDocName = docName;
    docInfo.pDatatype = dataType;

    BOOL bStatus = FALSE;
    HANDLE hPrinter = NULL;
    DWORD dwPrtJob = 0L;
    DWORD dwBytesWritten = 0L;

    bStatus = OpenPrinterW(printerName, &hPrinter, NULL);
    if (bStatus) {

        dwPrtJob = StartDocPrinterW(hPrinter, 1, (LPBYTE)&docInfo);

        if (dwPrtJob > 0) {
            // Send the data to the printer.
            bStatus = WritePrinter(hPrinter, lpData, dwCount, &dwBytesWritten);
        }

        EndDocPrinter(hPrinter);

        // Close the printer handle.
        bStatus = ClosePrinter(hPrinter);
    }
#else
    if (type == PRINT_TYPE::DEVICE) {
        QFile file(printName);
        if (file.open(QFile::WriteOnly)) {
            file.write(data.toLocal8Bit());
        }
    } else if (type == PRINT_TYPE::SPOOL) {
#ifndef NO_PRINTER_SPOOL
        int jobId = 0;
        jobId = cupsCreateJob(CUPS_HTTP_DEFAULT, printName.toStdString().c_str(), "Sultan print", 0, NULL);
        if (jobId > 0) {
            cupsStartDocument(CUPS_HTTP_DEFAULT, printName.toStdString().c_str(), jobId, "Sultan Document",
                              CUPS_FORMAT_RAW, true);
            cupsWriteRequestData(CUPS_HTTP_DEFAULT, data.toStdString().c_str(), data.length());
            cupsFinishDocument(CUPS_HTTP_DEFAULT, printName.toStdString().c_str());
        }
#endif
    } else if (type == PRINT_TYPE::USB) {
#ifdef USE_LIBUSB
        Usb::sendData(vendorId, produckId, data.toUtf8());
#endif
    }
#endif
}
