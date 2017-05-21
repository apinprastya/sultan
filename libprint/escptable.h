#ifndef ESCPTABLE_H
#define ESCPTABLE_H

#include "print_global.h"
#include <QString>
#include <QStringList>

namespace LibPrint {

class PRINTSHARED_EXPORT EscpTable
{
public:
    enum {
        ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
    };
    EscpTable(int width);
    void setColumnWidth(const QList<int> &colWidths);
    inline void setUseBorder(bool border) { mUseBorder = border; }
    void writeHeader(const QStringList &header);
    void writeRow(const QStringList &rowData);
    void setAlign(int column, int alignment);
    void end();
    QString data();

private:
    int mWidth;
    bool mUseBorder;
    QList<int> mColWidth;
    QList<int> mAlign;
    QString mData;
    QStringList mTempData;

    int getAlignment(int col);
    void append(QChar ch, int lenght);
    void write(const QString &val, int column);
    void write(const QString &val, int aligment, int width);
};

}

#endif // ESCPTABLE_H
