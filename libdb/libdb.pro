include(../external_library/easyloggingpp/easyloggingpp/easyloggingpp.pri)

QT += sql

TEMPLATE = lib
TARGET = db
target.path = $${LIBDIR}
INSTALLS += target

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += DB_LIBRARY_STATIC
} else {
    DEFINES += DB_LIBRARY
}
contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

macx {
    DESTDIR = ../bin/Turbin.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Turbin.app/Contents/Frameworks
} else {
    DESTDIR = ../bin
}
DLLDESTDIR = ../

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Turbin.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
}

HEADERS += \
    db_global.h \
    db.h \
    db_constant.h \
    dbresult.h \
    dbutil.h \
    querydb.h \
    queryhelper.h

SOURCES += \
    db.cpp \
    dbresult.cpp \
    dbutil.cpp \
    querydb.cpp \
    queryhelper.cpp

