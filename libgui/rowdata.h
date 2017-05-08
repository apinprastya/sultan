#ifndef ROWDATA_H
#define ROWDATA_H

#include "gui_global.h"
#include <QList>
#include <QMap>

namespace LibGUI  {

class TableItem;

class GUISHARED_EXPORT RowData
{
public:
    RowData();
    ~RowData();
    void clearAndRelease();
    void insert(int index, TableItem *item);
    void insert(int index, const QList<TableItem*> &list);
    void removeAndRelease(int index);
    TableItem *operator[](int index);
    TableItem *at(int index);
    TableItem *operator[](int index) const;
    bool exist(int index) const;
    int getIndexOfId(int id);
    inline int size() { return mData.size(); }
    inline TableItem *getItem(int index) { return mData[index]; }
    int getLogicalIndex(int internalIndex);

private:
    QList<TableItem*> mData;
    //key: index, value: lenght
    QMap<int, int> mOffset;

    int getInternalIndex(int index) const;
};

}
#endif // ROWDATA_H
