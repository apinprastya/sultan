#include "unitaction.h"

using namespace LibServer;

UnitAction::UnitAction():
    ServerAction("units", "id")
{
    mFlag = USE_TRANSACTION | SOFT_DELETE;
}
