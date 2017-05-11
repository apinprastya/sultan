/*
 * cashiertablemodel.h
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Turbin.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CASHIERTABLEMODEL_H
#define CASHIERTABLEMODEL_H

#include <QAbstractTableModel>

namespace LibGUI {

class CashierItem;

class CashierTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CashierTableModel(QObject *parent = nullptr);
    ~CashierTableModel();
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    void addItem(float count, const QString &name, const QString &barcode, const QVariantList &prices = QVariantList());
    void reset();

private:
    QList<CashierItem*> mData;
    QStringList mHeaders;
    QMap<QString, QVariantList> mPrices;

    float getTotalCount(const QString &barcode);
    void calculateTotal();
    int rowOfBarcode(const QString &barcode);
    void calculatePrices(CashierItem* item);

signals:
    void totalChanged(double total);
    void selectRow(const QModelIndex &index);
};

}
#endif // CASHIERTABLEMODEL_H
