#include "configutil.h"
#include "db.h"
#include "dbresult.h"
#include <QJsonDocument>

using namespace LibServer;
using namespace LibDB;

ConfigUtil::ConfigUtil()
{

}

QVariant ConfigUtil::getVariantConfig(LibDB::Db *db, int configId, const QVariant &defValue)
{
    DbResult res = db->clone()->reset()->where("id = ", configId)->get("configs");
    if(res.isEmpty()) return defValue;
    const QVariantMap &d = res.first();
    return d["value"];
}

QString ConfigUtil::getStringConfig(Db *db, int configId, const QString &defValue)
{
    return getVariantConfig(db, configId, defValue).toString();
}

int ConfigUtil::getIntConfig(Db *db, int configId, int defValue)
{
    return getVariantConfig(db, configId, defValue).toInt();
}

QJsonObject ConfigUtil::getJsonObjectConfig(Db *db, int configId, const QJsonObject &defValue)
{
    const QString &v = getVariantConfig(db, configId).toString();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(v.toUtf8(), &err);
    if(err.error != QJsonParseError::NoError)
        return defValue;
    return doc.object();
}

QJsonArray ConfigUtil::getJsonArrayConfig(Db *db, int configId, const QJsonArray &defValue)
{
    const QString &v = getVariantConfig(db, configId).toString();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(v.toUtf8(), &err);
    if(err.error != QJsonParseError::NoError)
        return defValue;
    return doc.array();
}

void ConfigUtil::setConfig(Db *db, int configId, const QVariant &value)
{
    db = db->clone()->reset();
    DbResult res = db->where("id = ", configId)->get("configs");
    if(res.isEmpty())
        db->insert("configs", QVariantMap{{"id", configId},{"value", value.toString()}});
    else
        db->where("id = ", configId)->update("configs", QVariantMap{{"value", value.toString()}});
}
