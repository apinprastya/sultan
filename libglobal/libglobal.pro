TEMPLATE = lib
TARGET = global
target.path = /usr/bin
INSTALLS += target

QT += concurrent core

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += GLOBAL_LIBRARY_STATIC
} else {
    DEFINES += GLOBAL_LIBRARY
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

SOURCES += \
    preference.cpp \
    message.cpp \
    futurewatcher.cpp \
    messagehandler.cpp \
    messagebus.cpp \
    usersession.cpp \
    permissionhelper.cpp \
    util.cpp \
    abstractsultangui.cpp

HEADERS += \ 
    global_global.h \
    preference.h \
    global_setting_const.h \
    global_constant.h \
    message.h \
    futurewatcher.h \
    messagehandler.h \
    messagebus.h \
    usersession.h \
    permissionhelper.h \
    util.h \
    abstractsultangui.h
