#include "purchaseitem.h"

using namespace LibGUI;

void PurchaseItem::fill(const QVariantMap &data) {
    id = data["id"].toInt();
    barcode = data["barcode"].toString();
    name = data["name"].toString();
    count = data["count"].toFloat();
    price = data["price"].toDouble();
    discount = data["discount"].toDouble();
    total = data["total"].toDouble();
    final = data["final"].toDouble();
}
