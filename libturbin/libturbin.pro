TEMPLATE = lib
TARGET = turbin
target.path = $${LIBDIR}
INSTALLS += target

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += TURBIN_LIBRARY_STATIC
} else {
    DEFINES += TURBIN_LIBRARY
}
contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

macx {
    DESTDIR = ../bin/Turbin.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Turbin.app/Contents/Frameworks
} else {
    DESTDIR = ../lib
}
DLLDESTDIR = ../

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Turbin.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    LIBS += -L$$OUT_PWD/../lib
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../lib $$QMAKE_LIBDIR
}


SOURCES += \ 
HEADERS += \ 
    turbin_global.h
