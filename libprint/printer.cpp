#include "printer.h"
#include "preference.h"
#include "global_constant.h"
#include "global_setting_const.h"
#include <QPrinterInfo>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <cups/cups.h>
#endif

using namespace LibPrint;
using namespace LibG;

static Printer *sInstance = Q_NULLPTR;

Printer::Printer()
{
}

Printer *Printer::instance()
{
    if(!sInstance)
        sInstance = new Printer();
    return sInstance;
}

void Printer::destroy()
{
    if(sInstance)
        delete sInstance;
    sInstance = Q_NULLPTR;
}

QStringList Printer::getPrintList()
{
    return QPrinterInfo::availablePrinterNames();
}

void Printer::print(const QString &printName, const QString &data, int type)
{
#ifdef Q_OS_WIN
    wchar_t printerName[256];
    printName.toWCharArray(printerName);
    printerName[printName.length()] = '\0';
    LPBYTE lpData;
    QByteArray ba = data.toLocal8Bit();
    lpData = (unsigned char*)(ba.data());
    DWORD dwCount = ba.length();

    DOC_INFO_1 docInfo;
    wchar_t docName[16], dataType[8];
    wcscpy_s(docName, 100, L"WS Document");
    wcscpy_s(dataType, 100, L"RAW");
    docInfo.pOutputFile = NULL;
    docInfo.pDocName = docName;
    docInfo.pDatatype = dataType;

    BOOL bStatus = FALSE;
    HANDLE hPrinter = NULL;
    DWORD dwPrtJob = 0L;
    DWORD dwBytesWritten = 0L;

    bStatus = OpenPrinterW(printerName, &hPrinter, NULL);
    if(bStatus) {

        dwPrtJob = StartDocPrinterW (
                        hPrinter,
                        1,
                        (LPBYTE)&docInfo);

        if (dwPrtJob > 0) {
                // Send the data to the printer.
                bStatus = WritePrinter (
                hPrinter,
                lpData,
                dwCount,
                &dwBytesWritten);
        }

        EndDocPrinter (hPrinter);

        // Close the printer handle.
        bStatus = ClosePrinter(hPrinter);
    }
#else
    if(type == PRINT_TYPE::DEVICE) {
        QFile file(printName);
        if(file.open(QFile::WriteOnly)) {
            file.write(data.toLocal8Bit());
        }
    } else {
        int jobId = 0;
        jobId = cupsCreateJob(CUPS_HTTP_DEFAULT, printName.toStdString().c_str(), "WS print", 0, NULL);
        if(jobId > 0) {
            cupsStartDocument(CUPS_HTTP_DEFAULT, printName.toStdString().c_str(), jobId, "WS Document", CUPS_FORMAT_RAW, true);
            cupsWriteRequestData(CUPS_HTTP_DEFAULT, data.toStdString().c_str(), data.length());
            cupsFinishDocument(CUPS_HTTP_DEFAULT, printName.toStdString().c_str());
        }
    }
#endif
}

