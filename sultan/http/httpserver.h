#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <HttpHandler.h>
#include <HttpServer.h>
#include <QObject>

class HttpServer : public QObject {
    Q_OBJECT
  public:
    HttpServer();
    void runServer(quint16 port);

  private:
    Pillow::HttpServer *mServer;
};

class HttpClientHandler : public Pillow::HttpHandler {
    Q_OBJECT

  public:
    HttpClientHandler(QObject *parent = nullptr);

  public slots:
    bool handleRequest(Pillow::HttpConnection *connection) override;
};

#endif // HTTPSERVER_H
