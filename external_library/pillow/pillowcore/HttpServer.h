#ifndef PILLOW_HTTPSERVER_H
#define PILLOW_HTTPSERVER_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QTCPSERVER_H
#include <QtNetwork/QTcpServer>
#endif //  QTCPSERVER_H
#ifndef QLOCALSERVER_H
#include <QtNetwork/QLocalServer>
#endif // QLOCALSERVER_H

namespace Pillow
{
	class HttpConnection;

	//
	// HttpServer
	//

	class HttpServerPrivate;

	class PILLOWCORE_EXPORT HttpServer : public QTcpServer
	{
		Q_OBJECT
		Q_PROPERTY(QHostAddress serverAddress READ serverAddress)
		Q_PROPERTY(int serverPort READ serverPort)
		Q_PROPERTY(bool listening READ isListening)
		Q_DECLARE_PRIVATE(HttpServer)
		HttpServerPrivate* d_ptr;

	private slots:
		void connection_closed(Pillow::HttpConnection* request);

	protected:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		void incomingConnection(int socketDescriptor);
#else
		void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
#endif
		HttpConnection* createHttpConnection();

	public:
		HttpServer(QObject* parent = 0);
		HttpServer(const QHostAddress& serverAddress, quint16 serverPort, QObject *parent = 0);
		~HttpServer();

	signals:
		void requestReady(Pillow::HttpConnection* connection); // There is a request ready to be handled on this connection.
	};

	//
	// HttpLocalServer
	//

	class PILLOWCORE_EXPORT HttpLocalServer : public QLocalServer
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(HttpServer)
		HttpServerPrivate* d_ptr;

	private slots:
		void this_newConnection();
		void connection_closed(Pillow::HttpConnection* request);

	public:
		HttpLocalServer(QObject* parent = 0);
		HttpLocalServer(const QString& serverName, QObject *parent = 0);

	signals:
		void requestReady(Pillow::HttpConnection* connection); // There is a request ready to be handled on this connection.
	};
}

#endif // PILLOW_HTTPSERVER_H
