TEMPLATE = lib
TARGET = socket
target.path = $${LIBDIR}
INSTALLS += target

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += SOCKET_LIBRARY_STATIC
} else {
    DEFINES += SOCKET_LIBRARY
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


SOURCES +=

HEADERS += \ 
    socket_global.h
