#ifndef UTIL_H
#define UTIL_H

#include "global_global.h"
#include <QDate>

namespace LibG {

class GLOBALSHARED_EXPORT Util
{
public:
    Util();
    static QDate getBeginningOfMonth();
    static QDate getEndOfMonth();
};

}
#endif // UTIL_H
