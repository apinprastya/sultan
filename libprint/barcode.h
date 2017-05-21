#ifndef BARCODE_H
#define BARCODE_H

#include "print_global.h"
#include <QString>
#include <QList>

namespace LibPrint {

struct BarcodePrintData {
    BarcodePrintData(const QString &name, const QString &barcode, int price, int copies):
        name(name), barcode(barcode), price(price), copies(copies){}
    QString name;
    QString barcode;
    int price;
    int copies;
};

class PRINTSHARED_EXPORT Barcode
{
public:
    Barcode();
    bool print(const QString &name, const QString &barcode, int price, int copies = 1);
    bool print(const QList<BarcodePrintData> &data);
};

}

#endif // BARCODE_H
