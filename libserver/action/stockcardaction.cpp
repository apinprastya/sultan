#include "stockcardaction.h"

using namespace LibServer;

StockCardAction::StockCardAction():
    ServerAction("stockcards", "id")
{
    mFlag = USE_TRANSACTION;
}
