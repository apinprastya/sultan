#include "escptable.h"
#include "print_constant.h"

using namespace LibPrint;

EscpTable::EscpTable(int width):
    mWidth(width)
{

}

void EscpTable::setColumnWidth(const QList<int> &colWidths)
{
    int availWidth = mWidth - colWidths.count() - 1;
    mColWidth.clear();
    mData.append(QLatin1Char(BORDER::TOP_LEFT));
    for(int i = 0; i < colWidths.size(); i++) {
        int w = availWidth * colWidths[i] / 100;
        mColWidth.append(w);
        mAlign.append(ALIGN_LEFT);
        append(QLatin1Char(BORDER::HORIZONTAL), w);
        if(i != colWidths.size() - 1)
            mData.append(QLatin1Char(BORDER::TOP_MIDDLE));
        else
            mData.append(QLatin1Char(BORDER::TOP_RIGHT));
    }
    mData.append(QLatin1Char(ESCP::LINE_FEED));
}

void EscpTable::writeHeader(const QStringList &header)
{
    if(header.size() != mColWidth.size())
        return;
    mData.append(QLatin1Char(BORDER::VERTICAL));
    for(int i = 0; i < header.size(); i++) {
        write(header[i], ALIGN_CENTER, mColWidth[i]);
        mData.append(QLatin1Char(BORDER::VERTICAL));
    }
    mData.append(QLatin1Char(ESCP::LINE_FEED));
    mData.append(QLatin1Char(BORDER::LEFT_MIDDLE));
    for(int i = 0; i < header.size(); i++) {
        append(QLatin1Char(BORDER::HORIZONTAL), mColWidth[i]);
        if(i != header.size() - 1)
            mData.append(QLatin1Char(BORDER::MIDDLE));
        else
            mData.append(QLatin1Char(BORDER::RIGHT_MIDDLE));
    }
    mData.append(QLatin1Char(ESCP::LINE_FEED));
}

void EscpTable::writeRow(const QStringList &rowData)
{
    mData.append(QLatin1Char(BORDER::VERTICAL));
    for(int i = 0; i < rowData.size(); i++) {
        write(rowData[i], i);
        mData.append(QLatin1Char(BORDER::VERTICAL));
    }
    mData.append(QLatin1Char(ESCP::LINE_FEED));
}

void EscpTable::setAlign(int column, int alignment)
{
    if(column >= mAlign.size())
        return;
    mAlign[column] = alignment;
}

void EscpTable::end()
{
    mData.append(QLatin1Char(BORDER::BOTTOM_LEFT));
    for(int i = 0; i < mColWidth.size(); i++) {
        append(QLatin1Char(BORDER::HORIZONTAL), mColWidth[i]);
        if(i != mColWidth.size() - 1)
            mData.append(QLatin1Char(BORDER::BOTTOM_MIDDLE));
        else
            mData.append(QLatin1Char(BORDER::BOTTOM_RIGHT));
    }
}

QString EscpTable::data()
{
    return mData;
}

int EscpTable::getAlignment(int col)
{
    return mAlign[col];
}

void EscpTable::append(QChar ch, int lenght)
{
    for(int i = 0; i < lenght; i++)
        mData.append(ch);
}

void EscpTable::write(const QString &val, int column)
{
    write(val, mAlign[column], mColWidth[column]);
}

void EscpTable::write(const QString &val, int aligment, int width)
{
    switch (aligment) {
        case ALIGN_LEFT: {
            if(val.length() > width) {
                QStringRef ref(&val, 0, width);
                mData.append(ref.toString());
            } else {
                QString s = val.leftJustified(width, QChar(0x20));
                mData.append(s);
            }
        } break;
        case ALIGN_CENTER: {
            if(val.length() > width) {
                QStringRef ref(&val, 0, width);
                mData.append(ref.toString());
            } else {
                QString s = val.rightJustified(val.length() + ((width - val.length()) / 2), QChar(0x20));
                s = s.leftJustified(width, QChar(0x20));
                mData.append(s);
            }
        } break;
        case ALIGN_RIGHT: {
            if(val.length() > width) {
                QStringRef ref(&val, 0, width);
                mData.append(ref.toString());
            } else {
                QString s = val.rightJustified(width, QChar(0x20));
                mData.append(s);
            }
        } break;
    }
}
