include(../libturbin/libturbin.pri)

QT       += core gui widgets

TARGET = Turbin
TEMPLATE = app

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Turbin.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    LIBS += -L$$OUT_PWD/../bin
}

win32 {
    DESTDIR = ../
} else {
    DESTDIR = ../bin
}

SOURCES += main.cpp

HEADERS  +=
