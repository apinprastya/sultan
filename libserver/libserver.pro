include(../libglobal/libglobal.pri)
include(../libdb/libdb.pri)
QXLSX_PARENTPATH=../external_library/QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../external_library/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../external_library/QXlsx/source/  # current QXlsx source path is ./source/
include(../external_library/QXlsx/QXlsx.pri)

TEMPLATE = lib
TARGET = server
target.path = /usr/bin
INSTALLS += target

QT += concurrent sql

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += SERVER_LIBRARY_STATIC
} else {
    DEFINES += SERVER_LIBRARY
}
contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

macx {
    DESTDIR = ../bin/Sultan.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
} else {
    DESTDIR = ../bin
}
DLLDESTDIR = ../

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
}

TRANSLATIONS = ../translation/libserver_id.ts

SOURCES += \
    serveraction.cpp \
    router.cpp \
    action/useraction.cpp \
    mainserver.cpp \
    action/suplieraction.cpp \
    action/itemaction.cpp \
    action/sellpriceactions.cpp \
    action/categoryaction.cpp \
    action/purchaseaction.cpp \
    action/purchaseitemaction.cpp \
    action/soldaction.cpp \
    action/solditemaction.cpp \
    action/customeraction.cpp \
    action/customerpointaction.cpp \
    action/cusomercreditaction.cpp \
    action/machineaction.cpp \
    action/rewardaction.cpp \
    action/rewardpoinaction.cpp \
    action/bankaction.cpp \
    action/transactionaction.cpp \
    action/purchasereturnaction.cpp \
    action/databaseaction.cpp \
    action/checkstockaction.cpp \
    action/unitaction.cpp \
    action/stockcardaction.cpp \
    action/itemlinkaction.cpp \
    util/itemutil.cpp \
    action/soldreturnaction.cpp \
    util/configutil.cpp \
    action/configaction.cpp

HEADERS += \ 
    server_global.h \
    serveraction.h \
    router.h \
    action/useraction.h \
    mainserver.h \
    action/suplieraction.h \
    action/itemaction.h \
    action/sellpriceactions.h \
    action/categoryaction.h \
    action/purchaseaction.h \
    action/purchaseitemaction.h \
    action/soldaction.h \
    action/solditemaction.h \
    action/customeraction.h \
    action/customerpointaction.h \
    action/cusomercreditaction.h \
    action/machineaction.h \
    action/rewardaction.h \
    action/rewardpoinaction.h \
    action/bankaction.h \
    action/transactionaction.h \
    action/purchasereturnaction.h \
    action/databaseaction.h \
    action/checkstockaction.h \
    action/unitaction.h \
    action/stockcardaction.h \
    action/itemlinkaction.h \
    util/itemutil.h \
    action/soldreturnaction.h \
    util/configutil.h \
    action/configaction.h
