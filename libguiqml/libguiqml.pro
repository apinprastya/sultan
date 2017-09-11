include(../external_library/o2/src/src.pri)
include(../libglobal/libglobal.pri)
include(../libprint/libprint.pri)
include(../libdb/libdb.pri)

TEMPLATE = lib
TARGET = guiqml
target.path = $${LIBDIR}
INSTALLS += target

QT += core gui widgets network sql quickcontrols2

CONFIG(USE_WEBENGINE) {
    QT += webenginewidgets
    DEFINES += USE_WEBENGINE
} else {
    QT += webkitwidgets
}

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += GUIQML_LIBRARY_STATIC
} else {
    DEFINES += GUIQML_LIBRARY
}

contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

DLLDESTDIR = ../

macx {
    DESTDIR = ../bin/Sultan.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    DESTDIR = ../bin
    LIBS += -L$$OUT_PWD/../bin
} else {
    DESTDIR = ../bin
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
}

#TRANSLATIONS = ../translation/libgui_id.ts

SOURCES += \
    mainwindowqml.cpp

HEADERS += \
    guiqml_global.h \
    mainwindowqml.h

FORMS +=
