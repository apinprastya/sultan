include(../external_library/easyloggingpp/easyloggingpp/easyloggingpp.pri)
include(../libglobal/libglobal.pri)
include(../libdb/libdb.pri)
include(../libserver/libserver.pri)
include(../libgui/libgui.pri)

QT       += core gui widgets sql websockets

TARGET = Sultan
TEMPLATE = app

CONFIG += c++11

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
    DESTDIR = ../bin
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
    RC_FILE = sultan.rc
    DESTDIR = ../bin/
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $$OUT_PWD/../bin/
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
    copymigration.commands = $$quote(cmd /c xcopy /S /I /Y $${PWD_WIN}\..\migrations $${DESTDIR_WIN}\migrations)
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    LIBS += -L$$OUT_PWD/../bin
    DESTDIR = ../bin
    copymigration.commands = $$quote(cp -R $${PWD}/../migrations $${OUT_PWD}/../bin/)
}

QMAKE_EXTRA_TARGETS += copymigration
POST_TARGETDEPS += copymigration

RESOURCES += sultan.qrc

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
