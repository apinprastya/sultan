/*
 * tablemodel.h
 * Copyright 2017 - ~, Apin <apin.klas@gmail.com>
 *
 * This file is part of Sultan.
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
#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include "gui_global.h"
#include "rowdata.h"
#include "messagehandler.h"
#include "querydb.h"
#include <QAbstractTableModel>
#include <functional>

namespace LibGUI {

struct HeaderFilter {
    int type;
    int compare;
    QVariant defValue;
};

class GUISHARED_EXPORT TableModel: public QAbstractTableModel, public LibG::MessageHandler
{
    Q_OBJECT
public:
    enum PageStatus { None, Loading, Loaded };
    enum Role { TitleRole = Qt::UserRole, FilterRole = Qt::UserRole + 1, FilterValueRole = Qt::UserRole + 2 };
    enum Filter { FilterEQ, FilterLike, FilterBetweenDate, FilterLikeNative, FilterCategory };
    TableModel(QObject *parent = nullptr, bool useStandartHeader = false);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    void reset();
    void addColumn(const QString &key, const QString &title, const int &align = Qt::AlignLeft, std::function<QVariant(TableItem*,const QString&)> formater = nullptr);
    void addColumnMoney(const QString &key, const QString &title);
    void addHeaderFilter(const QString &key, HeaderFilter filter);
    inline void setTypeCommand(const int &type, const int &command) { mTypeCommand = std::make_tuple(type, command); }
    inline void setTypeCommandOne(const int &type, const int &command) { mTypeCommandOne = std::make_tuple(type, command); }
    void setFilter(const QString &key, int type, const QVariant &value);
    void clearFilter();
    inline void setIdKey(const QString &key) { mIdKey = key; }
    inline LibDB::QueryDB *getQuery() { return &mQuery; }
    inline int getIndex(const QString &key) { return mColumns.indexOf(key); }
    void setSort(const QString &sort);
    inline void setAsLocal(bool value) { mIsLocal = value; }
    inline bool isLocal() { return mIsLocal; }
    inline void setTableItemTemplateFunc(std::function<TableItem*(void)> func) { mTemplateTableItemFunc = func; }
    void appendItem(TableItem* item);
    void removeItem(TableItem *item);
    inline RowData* getRowData() { return &mData; }
    inline void setDateTimeISO(bool value) { mDateTimeISO = value; }
    void setPerPageCount(int value, bool refresh = true);
    void slotPerPageCount(int value);

public slots:
    void refresh();
    void resfreshOne(const QVariant &id);
    void filterChanged(int index, const QVariant &value);

protected:
    void messageReceived(LibG::Message *msg) override;

protected:
    int mNumRow;
    bool mDateTimeISO = false;
    RowData mData;
    QList<QString> mHeaders;
    QList<QString> mColumns;
    QMap<QString, std::function<QVariant(TableItem*,const QString&)>> mFormater;
    QList<int> mAlignments;
    std::tuple<int, int> mTypeCommand;
    std::tuple<int, int> mTypeCommandOne;
    bool mIsLoaded;
    LibDB::QueryDB mQuery;
    QString mIdKey = QStringLiteral("id");
    QMap<QString, HeaderFilter> mHeaderFilter;
    bool mUseStandartHeader = false;
    bool mIsLocal = false;
    std::function<TableItem*(void)> mTemplateTableItemFunc = nullptr;
    int mPerPage = 10;
    int mCurrentPage = 0;
    int mRowCount = 0;

signals:
    void firstDataLoaded();
    void maxPageChanged(int value);
    void currentPageChanged(int value);

public slots:
    void loadPage(int page);

private:
    void readData(LibG::Message *msg);
    void readOneData(LibG::Message *msg);
};

}
#endif // TABLEMODEL_H
