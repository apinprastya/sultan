include(../external_library/easyloggingpp/easyloggingpp/easyloggingpp.pri)
include(../libglobal/libglobal.pri)

TEMPLATE = lib
TARGET = gui
target.path = $${LIBDIR}
INSTALLS += target

QT += core gui widgets

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += GUI_LIBRARY_STATIC
} else {
    DEFINES += GUI_LIBRARY
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


SOURCES += \
    mainwindow.cpp \
    tabwidget.cpp \
    ui/settingwidget.cpp \
    ui/userwidget.cpp \
    horizontalheader.cpp \
    headerwidget.cpp

HEADERS += \ 
    gui_global.h \
    mainwindow.h \
    tabwidget.h \
    ui/settingwidget.h \
    ui/userwidget.h \
    horizontalheader.h \
    headerwidget.h

FORMS += \
    mainwindow.ui \
    ui/settingwidget.ui \
    ui/userwidget.ui
