#ifndef ITEMUTIL_H
#define ITEMUTIL_H

#include <QStringList>
#include <QVariantMap>

namespace LibDB {
class Db;
}

namespace LibServer {

/*struct SoldItem {
    SoldItem(){}
    int soldId;
    float count;
    double price;
    double total;
    double discount;
    double final;
    double buyPrice;
    int flag;
    QString discountFormula;
    QString barcode;
    QString name;
};*/

class ItemUtil
{
public:
    ItemUtil(LibDB::Db *db);
    void updateBuyPrice(const QString &barcode);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id, bool recursive = true);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, QVariantMap data, bool recursive = true);
    void insertStock(const QString &barcode, const QString &number, int type, float count, int flag, int link_id, QVariantMap data, bool recursive = true);
    void updateStockCardCount(const QString &barcode, float count, int type, int link_id, bool recursive = true);

private:
    LibDB::Db *mDb;
    int mLevel = 0;
    QStringList mBarcodeInserted;
    QStringList mBarcodeUpdated;
};

}

#endif // ITEMUTIL_H
