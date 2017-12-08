#include "configaction.h"
#include "db.h"
#include "global_constant.h"
#include "util/configutil.h"

using namespace LibServer;
using namespace LibG;
using namespace LibDB;

ConfigAction::ConfigAction():
    ServerAction("configs", "id")
{
    mFunctionMap.insert(MSG_COMMAND::CONFIG_INSERT_UPDATE, std::bind(&ConfigAction::insertOrUpdate, this, std::placeholders::_1));
}

LibG::Message ConfigAction::insertOrUpdate(LibG::Message *msg)
{
    Message message(msg);
    const QVariantList &data = msg->data("data").toList();
    for(int i = 0; i < data.size(); i++) {
        const QVariantMap &d = data[i].toMap();
        ConfigUtil::setConfig(mDb, d["id"].toInt(), d["value"]);
    }
    return message;
}
