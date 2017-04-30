#ifndef PILLOW_HTTPSSERVER_H
#define PILLOW_HTTPSSERVER_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef PILLOW_HTTPSERVER_H
#include "HttpServer.h"
#endif // PILLOW_HTTPSERVER_H
#ifndef QSSLCERTIFICATE_H
#include <QtNetwork/QSslCertificate>
#endif // QSSLCERTIFICATE_H
#ifndef QSSLKEY_H
#include <QtNetwork/QSslKey>
#endif // QSSLKEY_H
#ifndef QSSLERROR_H
#include <QtNetwork/QSslError>
#endif // QSSLERROR_H

#if !defined(PILLOW_NO_SSL) && !defined(QT_NO_SSL)

namespace Pillow
{
	//
	// HttpsServer
	//

	class PILLOWCORE_EXPORT HttpsServer : public Pillow::HttpServer
	{
		Q_OBJECT
		QSslCertificate _certificate;
		QSslKey _privateKey;

	public slots:
		void sslSocket_encrypted();
		void sslSocket_sslErrors(const QList<QSslError>& sslErrors);

	protected:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		void incomingConnection(int socketDescriptor);
#else
		void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
#endif

	public:
		HttpsServer(QObject* parent = 0);
		HttpsServer(const QSslCertificate& certificate, const QSslKey& privateKey, const QHostAddress& serverAddress, quint16 serverPort, QObject *parent = 0);

		const QSslCertificate& certificate() const { return _certificate; }
		const QSslKey& privateKey() const { return _privateKey; }

	public slots:
		void setCertificate(const QSslCertificate& certificate);
		void setPrivateKey(const QSslKey& privateKey);
	};
}

#endif // !defined(PILLOW_NO_SSL) && !defined(QT_NO_SSL)

#endif // PILLOW_HTTPSSERVER_H
