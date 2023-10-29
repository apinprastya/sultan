include(../libglobal/libglobal.pri)

QT += sql concurrent

TEMPLATE = lib
TARGET = db
target.path = /usr/bin
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

HEADERS += \
    db_global.h \
    db.h \
    db_constant.h \
    dbresult.h \
    dbutil.h \
    querydb.h \
    queryhelper.h \
    migration.h

SOURCES += \
    db.cpp \
    dbresult.cpp \
    dbutil.cpp \
    querydb.cpp \
    queryhelper.cpp \
    migration.cpp

