include(../libglobal/libglobal.pri)
include(../libdb/libdb.pri)

TEMPLATE = lib
TARGET = server
target.path = $${LIBDIR}
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
    copytr.commands = $$quote(cp -R $${PWD}/libserver_id.qm $${OUT_PWD}/../bin/)
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $$OUT_PWD/../bin/
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
    copytr.commands = $$quote(cmd /c xcopy /S /I /Y $${PWD_WIN}\libserver_id.qm $${DESTDIR_WIN})
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    copytr.commands = $$quote(cp -R $${PWD}/libserver_id.qm $${OUT_PWD}/../bin/)
}

TRANSLATIONS = libserver_id.ts

QMAKE_EXTRA_TARGETS += copytr
POST_TARGETDEPS += copytr

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
    action/transactionaction.cpp

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
    action/transactionaction.h
