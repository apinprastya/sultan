INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/parser/parser.c \
	$$PWD/parser/http_parser.c \
	$$PWD/HttpServer.cpp \
	$$PWD/HttpHandler.cpp \
        #$$PWD/HttpHandlerQtScript.cpp \
	$$PWD/HttpHelpers.cpp \
	$$PWD/HttpsServer.cpp \
	$$PWD/HttpHandlerSimpleRouter.cpp \
	$$PWD/HttpConnection.cpp \
	$$PWD/HttpHandlerProxy.cpp \
	$$PWD/HttpClient.cpp \
	$$PWD/HttpHeader.cpp

HEADERS += \
	$$PWD/parser/parser.h \
	$$PWD/parser/http_parser.h \
	$$PWD/HttpServer.h \
	$$PWD/HttpHandler.h \
        #$$PWD/HttpHandlerQtScript.h \
	$$PWD/HttpHelpers.h \
	$$PWD/HttpsServer.h \
	$$PWD/HttpHandlerSimpleRouter.h \
	$$PWD/HttpConnection.h \
	$$PWD/HttpHandlerProxy.h \
	$$PWD/ByteArrayHelpers.h \
	$$PWD/private/ByteArray.h \
	$$PWD/HttpClient.h \
	$$PWD/pch.h \
	$$PWD/HttpHeader.h \
	$$PWD/PillowCore.h
