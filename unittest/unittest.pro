QT       += core gui widgets sql printsupport concurrent serialport network testlib

TARGET = UnitTest
TEMPLATE = app

CONFIG += c++11

QXLSX_PARENTPATH=../external_library/QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../external_library/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../external_library/QXlsx/source/  # current QXlsx source path is ./source/
include(../external_library/QXlsx/QXlsx.pri)
include(../libglobal/libglobal_src.pri)
include(../libprint/libprint_src.pri)
include(../libdb/libdb_src.pri)
include(../libserver/libserver_src.pri)
include(../libgui/libgui_src.pri)

LIBS += -lcups

SOURCES += main.cpp \
    simple.cpp

HEADERS  += \
    simple.h

FORMS +=

target.path = /usr/bin
INSTALLS += target
