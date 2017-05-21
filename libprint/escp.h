#ifndef ESCP_H
#define ESCP_H

#include "print_global.h"
#include <QList>
#include <QString>

namespace LibPrint {

class PRINTSHARED_EXPORT Escp
{
public:
    enum {
        CPI = 1,
        BOLD = 8,
        DOUBLE_HEIGHT = 16,
        DOUBLE_WIDTH = 32
    };
    enum {
        SIMPLE,
        FULL
    };
    enum {
        LEFT, CENTER, RIGHT
    };
    Escp(int type, int width10, int width12, int width15 = 0);
    void setWidth(int width);
    void line(const QChar &ch = QChar(0x2d));
    void newLine(int line = 1);
    void bold(bool bold = true);
    void doubleHeight(bool value = true);
    void doubleWidth(bool value = true);
    void cpi10();
    void cpi12();
    void cpi15();
    void column(const QList<int> col);
    void leftText(const QString &str, bool overflow = false);
    void centerText(const QString &str, bool overflow = false);
    void rightText(const QString &str, bool overflow = false);
    inline QString data() const { return mData; }
    inline int numLine() { return mNumLine; }
    inline void append(const QString &data) { mData.append(data); }
    void openDrawer();
    static QString openDrawerCommand();

private:
    int mType;
    int mMaster;
    int mWidth10;
    int mWidth12;
    int mWidth15;
    int mWidth;
    int mCurCol;
    QList<int> mColumn;
    int mRestWidth;
    QString mData;
    int mNumLine;

    void init();
    int getCurrentWidth(int col);
    bool isTrue(int flag);
    void writeText(const QString &str, int width, int alignment);
};

}
#endif // ESCP_H
