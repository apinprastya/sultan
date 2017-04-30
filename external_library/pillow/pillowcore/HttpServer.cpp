#include "HttpServer.h"
#include "HttpConnection.h"
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QLocalSocket>
using namespace Pillow;

//
// HttpServer
//

namespace Pillow
{
	class HttpServerPrivate
	{
	public:
		enum { MaximumReserveCount = 25 };

	public:
		QObject* q_ptr;
		QList<HttpConnection*> reservedConnections;

	public:
		HttpServerPrivate(QObject* server)
			: q_ptr(server)
		{
			for (int i = 0; i < MaximumReserveCount; ++i)
				reservedConnections << createConnection();
		}

		~HttpServerPrivate()
		{
			while (!reservedConnections.isEmpty())
				delete reservedConnections.takeLast();
		}

		HttpConnection* createConnection()
		{
			HttpConnection* connection = new HttpConnection(q_ptr);
			QObject::connect(connection, SIGNAL(requestReady(Pillow::HttpConnection*)), q_ptr, SIGNAL(requestReady(Pillow::HttpConnection*)));
			QObject::connect(connection, SIGNAL(closed(Pillow::HttpConnection*)), q_ptr, SLOT(connection_closed(Pillow::HttpConnection*)));
			return connection;
		}

		HttpConnection* takeConnection()
		{
			if (reservedConnections.isEmpty())
				return createConnection();
			else
				return reservedConnections.takeLast();
		}

		void putConnection(HttpConnection* connection)
		{
			while (reservedConnections.size() >= MaximumReserveCount)
				delete reservedConnections.takeLast();

			reservedConnections.append(connection);
		}
	};
}

HttpServer::HttpServer(QObject *parent)
: QTcpServer(parent), d_ptr(new HttpServerPrivate(this))
{
	setMaxPendingConnections(128);
}

HttpServer::HttpServer(const QHostAddress &serverAddress, quint16 serverPort, QObject *parent)
:	QTcpServer(parent), d_ptr(new HttpServerPrivate(this))
{
	setMaxPendingConnections(128);
	if (!listen(serverAddress, serverPort))
		qWarning() << QString("HttpServer::HttpServer: could not bind to %1:%2 for listening: %3").arg(serverAddress.toString()).arg(serverPort).arg(errorString());
}

HttpServer::~HttpServer()
{
	delete d_ptr;
}

#if QT_VERSION < 0x050000
void HttpServer::incomingConnection(int socketDescriptor)
#else
void HttpServer::incomingConnection(qintptr socketDescriptor)
#endif
{
	QTcpSocket* socket = new QTcpSocket(this);
	if (socket->setSocketDescriptor(socketDescriptor))
	{
		addPendingConnection(socket);
		nextPendingConnection();
		createHttpConnection()->initialize(socket, socket);
	}
	else
	{
        qWarning() << "HttpServer::incomingConnection: failed to set socket descriptor '" << socketDescriptor << "' on socket.";
		delete socket;
	}
}

void HttpServer::connection_closed(Pillow::HttpConnection *connection)
{
	connection->inputDevice()->deleteLater();
	d_ptr->putConnection(connection);
}

HttpConnection* Pillow::HttpServer::createHttpConnection()
{
	return d_ptr->takeConnection();
}

//
// HttpLocalServer
//

HttpLocalServer::HttpLocalServer(QObject *parent)
	: QLocalServer(parent), d_ptr(new HttpServerPrivate(this))
{
	setMaxPendingConnections(128);
	connect(this, SIGNAL(newConnection()), this, SLOT(this_newConnection()));
}

HttpLocalServer::HttpLocalServer(const QString& serverName, QObject *parent /*= 0*/)
	: QLocalServer(parent), d_ptr(new HttpServerPrivate(this))
{
	setMaxPendingConnections(128);
	connect(this, SIGNAL(newConnection()), this, SLOT(this_newConnection()));

	if (!listen(serverName))
		qWarning() << QString("HttpLocalServer::HttpLocalServer: could not bind to %1 for listening: %2").arg(serverName).arg(errorString());
}

void HttpLocalServer::this_newConnection()
{
	QIODevice* device = nextPendingConnection();
	d_ptr->takeConnection()->initialize(device, device);
}

void HttpLocalServer::connection_closed(Pillow::HttpConnection *connection)
{
	connection->inputDevice()->deleteLater();
	d_ptr->putConnection(connection);
}
