#include "httpserver.h"
#include "HttpConnection.h"
#include <QDebug>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QUuid>

HttpServer::HttpServer() {}

void HttpServer::runServer(quint16 port) {
    // qDebug() << QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    mServer = new Pillow::HttpServer(QHostAddress::Any, port);
    auto handlerStack = new Pillow::HttpHandlerStack(mServer);
    new HttpClientHandler(handlerStack);
    new Pillow::HttpHandlerFile(QStandardPaths::writableLocation(QStandardPaths::TempLocation), handlerStack);
    connect(mServer, SIGNAL(requestReady(Pillow::HttpConnection *)), handlerStack,
            SLOT(handleRequest(Pillow::HttpConnection *)));
}

HttpClientHandler::HttpClientHandler(QObject *parent) : Pillow::HttpHandler(parent) {}

bool HttpClientHandler::handleRequest(Pillow::HttpConnection *connection) {
    QString method = QString::fromUtf8(connection->requestMethod());
    if (method == "POST") {
        QString dirName = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        QDir dir(dirName);
        QFile file(dir.absoluteFilePath(QUuid::createUuid().toString()));
        if (file.open(QFile::WriteOnly)) {
            file.write(connection->requestContent());
            file.close();
        }
        connection->writeResponseString(200, Pillow::HttpHeaderCollection(), file.fileName());
        return true;
    }
    return false;
}
