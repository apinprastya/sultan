include(../libglobal/libglobal.pri)

QT += printsupport widgets

TEMPLATE = lib
TARGET = print
target.path = $${LIBDIR}
INSTALLS += target

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += PRINT_LIBRARY_STATIC
} else {
    DEFINES += PRINT_LIBRARY
}
contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

macx {
    DESTDIR = ../bin/Sultan.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
} else {
    DESTDIR = ../bin/
}

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
    LIBS += -lcups
} else:win32 {
    LIBS += -lKernel32 -lwinspool
    LIBS += -L$$OUT_PWD/../bin/
} else {
    LIBS += -lcups
    QMAKE_LIBDIR = $$OUT_PWD/../bin/ $$QMAKE_LIBDIR
}

SOURCES += \
    escp.cpp \
    printer.cpp \
    printtestdialog.cpp \
    escptable.cpp \
    barcode.cpp

HEADERS += \
    print_constant.h \
    print_global.h \
    escp.h \
    printer.h \
    printtestdialog.h \
    escptable.h \
    barcode.h

FORMS += \
    printtestdialog.ui
