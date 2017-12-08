#ifndef CONFIGUTIL_H
#define CONFIGUTIL_H

#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

namespace LibDB {
class Db;
}

namespace LibServer {

class ConfigUtil
{
public:
    ConfigUtil();
    static QVariant getVariantConfig(LibDB::Db *db, int configId, const QVariant &defValue = QVariant());
    static QString getStringConfig(LibDB::Db *db, int configId, const QString &defValue = QString());
    static int getIntConfig(LibDB::Db *db, int configId, int defValue = 0);
    static QJsonObject getJsonObjectConfig(LibDB::Db *db, int configId, const QJsonObject &defValue = QJsonObject());
    static QJsonArray getJsonArrayConfig(LibDB::Db *db, int configId, const QJsonArray &defValue = QJsonArray());
    static void setConfig(LibDB::Db *db, int configId, const QVariant &value);
};

}

#endif // CONFIGUTIL_H
