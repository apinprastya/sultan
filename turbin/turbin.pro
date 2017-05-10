include(../external_library/easyloggingpp/easyloggingpp/easyloggingpp.pri)
include(../libglobal/libglobal.pri)
include(../libdb/libdb.pri)
include(../libserver/libserver.pri)
include(../libgui/libgui.pri)

QT       += core gui widgets sql websockets

TARGET = Turbin
TEMPLATE = app

CONFIG += c++11

#Sorry for doing this
macx {
    DEFINES += "MIGRATION_FOLDER=\\\"/Users/apinprastya/Projects/qt/Turbin/migrations\\\""
} else {
    DEFINES += "MIGRATION_FOLDER=\\\"/media/data/Project/Qt/turbin/migrations\\\""
}

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

RESOURCES += turbin.qrc

SOURCES += main.cpp \
    core.cpp \
    gui/splash.cpp \
    gui/settingdialog.cpp \
    socket/socketmanager.cpp \
    socket/socketclient.cpp \
    socket/sockethandler.cpp \
    gui/logindialog.cpp

HEADERS  += \
    core.h \
    gui/splash.h \
    gui/settingdialog.h \
    socket/socketmanager.h \
    socket/socketclient.h \
    socket/sockethandler.h \
    gui/logindialog.h

FORMS += \
    gui/splash.ui \
    gui/settingdialog.ui \
    gui/logindialog.ui
