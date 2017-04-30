#ifndef PILLOW_HTTPCONNECTION_H
#define PILLOW_HTTPCONNECTION_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QOBJECT_H
#include <QObject>
#endif // QOBJECT_H
#ifndef QHOSTADDRESS_H
#include <QtNetwork/QHostAddress>
#endif // QHOSTADDRESS_H
#ifndef PILLOW_HTTPHEADER_H
#include "HttpHeader.h"
#endif // PILLOW_HTTPHEADER_H
class QIODevice;

namespace Pillow
{
	typedef QPair<QString, QString> HttpParam;
	typedef QVector<HttpParam> HttpParamCollection;
	class HttpConnectionPrivate;

	//
	// HttpConnection
	//

	class PILLOWCORE_EXPORT HttpConnection : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(State state READ state)
		Q_PROPERTY(QByteArray requestMethod READ requestMethod NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestUri READ requestUri NOTIFY requestReady)
		Q_PROPERTY(QString requestUriDecoded READ requestUriDecoded NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestPath READ requestPath NOTIFY requestReady)
		Q_PROPERTY(QString requestPathDecoded READ requestPathDecoded NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestQueryString READ requestQueryString NOTIFY requestReady)
		Q_PROPERTY(QString requestQueryStringDecoded READ requestQueryStringDecoded NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestFragment READ requestFragment NOTIFY requestReady)
		Q_PROPERTY(QString requestFragmentDecoded READ requestFragmentDecoded NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestHttpVersion READ requestHttpVersion NOTIFY requestReady)
		Q_PROPERTY(QByteArray requestContent READ requestContent NOTIFY requestReady)

	public:
		enum State { Uninitialized, ReceivingHeaders, ReceivingContent, SendingHeaders, SendingContent, Completed, Flushing, Closed };
		enum { MaximumRequestHeaderLength = 32 * 1024 };
		enum { MaximumRequestContentLength = 128 * 1024 * 1024 };
		Q_ENUMS(State);

	public:
		HttpConnection(QObject* parent = 0);
		~HttpConnection();

		void initialize(QIODevice* inputDevice, QIODevice* outputDevice = 0);

		QIODevice* inputDevice() const;
		QIODevice* outputDevice() const;
		State state() const;

		QHostAddress remoteAddress() const;

		// Request members. Note: the underlying shared QByteArray data remains valid until either the requestCompleted()
		// or closed() signals are emitted. Call detach() on your copy of the QByteArrays if you wish to keep it longer.
		const QByteArray& requestMethod() const;
		const QByteArray& requestUri() const;
		const QByteArray& requestFragment() const;
		const QByteArray& requestPath() const;
		const QByteArray& requestQueryString() const;
		const QByteArray& requestHttpVersion() const;
		const QByteArray& requestContent() const;

		// Request members, decoded version. Use those rather than manually decoding the raw data returned by the methods
		// above when decoded values are desired (they are cached).
		const QString& requestUriDecoded() const;
		const QString& requestFragmentDecoded() const;
		const QString& requestPathDecoded() const;
		const QString& requestQueryStringDecoded() const;

		// Request headers. As for field above, the underlying shared QByteArray data remains valid until either the requestCompleted()
		// or closed() signals are emitted.
		Q_INVOKABLE const Pillow::HttpHeaderCollection& requestHeaders() const;
		Q_INVOKABLE const QByteArray & requestHeaderValue(const QByteArray& field);

		// Request params.
		const Pillow::HttpParamCollection& requestParams();
		Q_INVOKABLE QString requestParamValue(const QString& name);
		Q_INVOKABLE void setRequestParam(const QString& name, const QString& value);

	public slots:
		// Response members.
		void writeResponse(int statusCode = 200, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& content = QByteArray());
		void writeResponseString(int statusCode = 200, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QString& content = QString());
		void writeHeaders(int statusCode = 200, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());
		void writeContent(const QByteArray& content);
		void endContent();

		void flush();
		void close(); // Close communication channels right away, no matter if a response was sent or not.

		// Information about the currentlly outgoing response. Valid between a call to writeHeaders until
		// the requestCompleted signal is emitted.
		int responseStatusCode() const;
		qint64 responseContentLength() const;

	signals:
		void requestReady(Pillow::HttpConnection* self);     // The request is ready to be processed, all request headers and content have been received.
		void requestCompleted(Pillow::HttpConnection* self); // The response is completed, all response headers and content have been sent.
		void closed(Pillow::HttpConnection* self);			 // The connection is closing, no further requests will arrive on this object.

	private slots:
		void processInput();
		void drain();

	private:
		Q_DECLARE_PRIVATE(HttpConnection)
		Pillow::HttpConnectionPrivate* d_ptr;
	};
}

#endif // PILLOW_HTTPCONNECTION_H
