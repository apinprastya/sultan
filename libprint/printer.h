#ifndef PRINTER_H
#define PRINTER_H

#include "print_global.h"
#include <QStringList>

namespace LibPrint {

class PRINTSHARED_EXPORT Printer
{
private:
    Printer();

public :
    static Printer *instance();
    static void destroy();
    QStringList getPrintList();
    void print(const QString &printName, const QString &data, int type = 1);
};

}
#endif // PRINTER_H
