TARGET = easyloggingpp
TEMPLATE = lib
macx {
    DESTDIR = ../../../bin/WS.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../../../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../../../bin/WS.app/Contents/Frameworks
} else {
    DESTDIR = ../../../bin
}

win32 {
    DEFINES += AUTO_INITIALIZE_EASYLOGGINGPP
    DEFINES += ELPP_AS_DLL
}

DEFINES += QT_NO_CAST_FROM_ASCII \
    QT_NO_CAST_TO_ASCII \
    ELPP_QT_LOGGING

QT       += core

CONFIG += c++11

SOURCES += \
	easylogging++.cc

HEADERS += \
	easylogging++.h
