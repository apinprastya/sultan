#include "itemlinkaction.h"
#include "db.h"

using namespace LibServer;
using namespace LibDB;

ItemLinkAction::ItemLinkAction():
    ServerAction("itemlinks", "id")
{
    mFlag = USE_TRANSACTION;
}

void ItemLinkAction::selectAndJoin()
{
    mDb->select("itemlinks.*, (select name from items where barcode = itemlinks.barcode) as name, \
                (select name from items where barcode = itemlinks.barcode_link) as name_link");
}
