#include "HttpsServer.h"
#include "HttpConnection.h"
#include <QtNetwork/QSslSocket>

#if !defined(PILLOW_NO_SSL) && !defined(QT_NO_SSL)

using namespace Pillow;

//
// HttpsServer
//

HttpsServer::HttpsServer(QObject *parent)
	: HttpServer(parent)
{
}

HttpsServer::HttpsServer(const QSslCertificate& certificate, const QSslKey& privateKey, const QHostAddress &serverAddress, quint16 serverPort, QObject *parent)
	: HttpServer(serverAddress, serverPort, parent), _certificate(certificate), _privateKey(privateKey)
{
}

void HttpsServer::setCertificate(const QSslCertificate &certificate)
{
	_certificate = certificate;
}

void HttpsServer::setPrivateKey(const QSslKey &privateKey)
{
	_privateKey = privateKey;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void HttpsServer::incomingConnection(int socketDescriptor)
#else
void HttpsServer::incomingConnection(qintptr socketDescriptor)
#endif
{
	QSslSocket* sslSocket = new QSslSocket(this);
	if (sslSocket->setSocketDescriptor(socketDescriptor))
	{
		sslSocket->setPrivateKey(privateKey());
		sslSocket->setLocalCertificate(certificate());
		sslSocket->startServerEncryption();
		connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslSocket_sslErrors(QList<QSslError>)));
		connect(sslSocket, SIGNAL(encrypted()), this, SLOT(sslSocket_encrypted()));
		addPendingConnection(sslSocket);
		nextPendingConnection();
		createHttpConnection()->initialize(sslSocket, sslSocket);
	}
	else
	{
		qWarning() << "HttpsServer::incomingConnection: failed to set socket descriptor '" << socketDescriptor << "' on ssl socket.";
		delete sslSocket;
	}
}

void HttpsServer::sslSocket_sslErrors(const QList<QSslError>&)
{
}

void HttpsServer::sslSocket_encrypted()
{
}

#endif // !defined(PILLOW_NO_SSL) && !defined(QT_NO_SSL)
