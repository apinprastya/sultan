QT       += core gui widgets sql printsupport concurrent serialport network

CONFIG(USE_EMBED_BROWSER) {
    DEFINES+=USE_EMBED_BROWSER
    CONFIG(USE_WEBENGINE) {
        QT += webenginewidgets
        DEFINES += USE_WEBENGINE
    } else {
        QT += webkitwidgets
    }
}

TARGET = Sultan
TEMPLATE = app

CONFIG += c++11

contains(CONFIG, SINGLEBIN) {
    QXLSX_PARENTPATH=../external_library/QXlsx/         # current QXlsx path is . (. means curret directory)
    QXLSX_HEADERPATH=../external_library/QXlsx/header/  # current QXlsx header path is ./header/
    QXLSX_SOURCEPATH=../external_library/QXlsx/source/  # current QXlsx source path is ./source/
    include(../external_library/QXlsx/QXlsx.pri)
    include(../libglobal/libglobal_src.pri)
    include(../libprint/libprint_src.pri)
    include(../libdb/libdb_src.pri)
    include(../libserver/libserver_src.pri)
    include(../libgui/libgui_src.pri)
} else {
    include(../libglobal/libglobal.pri)
    include(../libprint/libprint.pri)
    include(../libdb/libdb.pri)
    include(../libserver/libserver.pri)
    include(../libgui/libgui.pri)
}

CONFIG(NO_PRINTER_DEVICE) {
    DEFINES+=NO_PRINTER_DEVICE
}

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
    contains(CONFIG, SINGLEBIN) {
        DEFINES+=SINGLEBIN
        LIBS += -lcups
    }
    DESTDIR = ../bin
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
    contains(CONFIG, SINGLEBIN) {
        DEFINES+=SINGLEBIN
        !contains(CONFIG, NO_PRINTER_SPOOL) {
            LIBS += -lKernel32 -lwinspool
        } else {
            DEFINES+=NO_PRINTER_SPOOL
        }
    }
    RC_FILE = sultan.rc
    DESTDIR = ../bin/
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $$OUT_PWD/../bin/
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    LIBS += -L$$OUT_PWD/../bin
    contains(CONFIG, SINGLEBIN) {
        DEFINES+=SINGLEBIN
        !contains(CONFIG, NO_PRINTER_SPOOL) {
            LIBS += -lcups
        } else {
            DEFINES+=NO_PRINTER_SPOOL
        }
    }
    contains(CONFIG, USE_LIBUSB) {
        DEFINES+=USE_LIBUSB
        LIBS += -lusb-1.0 -ludev
    }
    DESTDIR = ../bin
    copysh.commands = $$quote(cp -R $${PWD}/../script/Sultan.sh $${OUT_PWD}/../bin/)
    QMAKE_EXTRA_TARGETS += copysh
    POST_TARGETDEPS += copysh
}

RESOURCES += sultan.qrc sqlmigration.qrc translation.qrc

TRANSLATIONS = ../translation/sultan_id.ts

SOURCES += main.cpp \
    core.cpp \
    socket/socketmanager.cpp \
    socket/socketclient.cpp \
    socket/sockethandler.cpp \
    socket/wraptcpsocket.cpp

HEADERS  += \
    core.h \
    socket/socketmanager.h \
    socket/socketclient.h \
    socket/sockethandler.h \
    socket/wraptcpsocket.h

FORMS +=

target.path = /usr/bin
INSTALLS += target