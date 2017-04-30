#ifndef PILLOW_HTTPHANDLERPROXY_H
#define PILLOW_HTTPHANDLERPROXY_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef PILLOW_HTTPHANDLER_H
#include "HttpHandler.h"
#endif // PILLOW_HTTPHANDLER_H
#ifndef QURL_H
#include <QtCore/QUrl>
#endif // QURL_H
#ifndef QNETWORKACCESSMANAGER_H
#include <QtNetwork/QNetworkAccessManager>
#endif // QNETWORKACCESSMANAGER_H
#ifndef QNETWORKREQUEST_H
#include <QtNetwork/QNetworkRequest>
#endif // QNETWORKREQUEST_H

namespace Pillow
{
	class ElasticNetworkAccessManager;
	class HttpHandlerProxyPipe;

	class PILLOWCORE_EXPORT HttpHandlerProxy : public Pillow::HttpHandler
	{
		Q_OBJECT
		QUrl _proxiedUrl;

	protected:
		ElasticNetworkAccessManager* _networkAccessManager;

	public:
		HttpHandlerProxy(QObject *parent = 0);
		HttpHandlerProxy(const QUrl &proxiedUrl, QObject *parent = 0);

		const QUrl& proxiedUrl() const { return _proxiedUrl; }
		void setProxiedUrl(const QUrl& proxiedUrl);

		inline ElasticNetworkAccessManager *networkAccessManager() const { return _networkAccessManager; }

	protected:
		virtual QNetworkReply* createProxiedReply(Pillow::HttpConnection* request, QNetworkRequest proxiedRequest);
		virtual Pillow::HttpHandlerProxyPipe* createPipe(Pillow::HttpConnection* request, QNetworkReply* proxiedReply);

	public:
		virtual bool handleRequest(Pillow::HttpConnection *request);
	};

	class PILLOWCORE_EXPORT HttpHandlerProxyPipe : public QObject
	{
		Q_OBJECT

	protected:
		Pillow::HttpConnection* _request;
		QNetworkReply* _proxiedReply;
		bool _headersSent;
		bool _broken;

	public:
		HttpHandlerProxyPipe(Pillow::HttpConnection* request, QNetworkReply* proxiedReply);
		~HttpHandlerProxyPipe();

		Pillow::HttpConnection* request() const { return _request; }
		QNetworkReply* proxiedReply() const { return _proxiedReply; }

	protected slots:
		virtual void teardown();
		virtual void sendHeaders();
		virtual void pump(const QByteArray& data);

	private slots:
		void proxiedReply_readyRead();
		void proxiedReply_finished();
	};

	class PILLOWCORE_EXPORT ElasticNetworkAccessManager : public QNetworkAccessManager
	{
		Q_OBJECT

	public:
		ElasticNetworkAccessManager(QObject* parent);
		~ElasticNetworkAccessManager();

	protected:
		virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
	};
}

#endif // PILLOW_HTTPHANDLERPROXY_H
