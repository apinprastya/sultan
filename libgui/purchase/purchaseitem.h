#ifndef PURCHASEITEM_H
#define PURCHASEITEM_H

#include <QVariantMap>

namespace LibGUI {

struct PurchaseItem
{
    int id;
    float count;
    double price;
    double discount;
    double total;
    double final;
    QString barcode;
    QString name;
    void fill(const QVariantMap &data);
};

}

#endif // PURCHASEITEM_H
