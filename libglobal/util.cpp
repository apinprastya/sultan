#include "util.h"

using namespace LibG;

Util::Util()
{

}

QDate Util::getBeginningOfMonth()
{
    auto now = QDate::currentDate();
    QDate retVal;
    retVal.setDate(now.year(), now.month(), 1);
    return retVal;
}

QDate Util::getEndOfMonth()
{
    auto retVal = getBeginningOfMonth();
    retVal = retVal.addMonths(1);
    retVal = retVal.addDays(-1);
    return retVal;
}
