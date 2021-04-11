include(../libglobal/libglobal.pri)

QT += printsupport serialport

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
    !contains(CONFIG, NO_PRINTER_SPOOL) {
        LIBS += -lcups
    } else {
        DEFINES += NO_PRINTER_SPOOL
    }
    contains(CONFIG, USE_LIBUSB) {
        DEFINES+=USE_LIBUSB
        LIBS += -lusb-1.0
    }
    QMAKE_LIBDIR = $$OUT_PWD/../bin/ $$QMAKE_LIBDIR
}

SOURCES += \
    customerdisplay.cpp \
    customerdisplaymanager.cpp \
    escp.cpp \
    printer.cpp \
    escptable.cpp \
    barcode.cpp \
    usb.cpp

HEADERS += \
    customerdisplay.h \
    customerdisplaymanager.h \
    print_constant.h \
    print_global.h \
    escp.h \
    printer.h \
    escptable.h \
    barcode.h \
    usb.h

FORMS +=
