#ifndef PILLOW_HTTPCLIENT_H
#define PILLOW_HTTPCLIENT_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QOBJECT_H
#include <QtCore/QObject>
#endif // QOBJECT_H
#ifndef QURL_H
#include <QtCore/QUrl>
#endif // QURL_H
#ifndef QTIMESTAMP_H
#include <QtCore/QElapsedTimer>
#endif // QTIMESTAMP_H
#ifndef QNETWORKACCESSMANAGER_H
#include <QtNetwork/QNetworkAccessManager>
#endif // QNETWORKACCESSMANAGER_H
#ifndef PILLOW_HTTPCONNECTION_H
#include "HttpConnection.h"
#endif // PILLOW_HTTPCONNECTION_H
#ifndef http_parser_h
#include "parser/http_parser.h"
#endif // http_parser_h

class QIODevice;
class QTcpSocket;
namespace Pillow { class ContentTransformer; }

namespace Pillow
{
	//
	// Pillow::HttpRequestWriter
	//
	// A small utility class to build HTTP requests and write them to a device.
	//
	// Reentrant. Not thread safe.
	//
	class PILLOWCORE_EXPORT HttpRequestWriter
	{
	public:
		explicit HttpRequestWriter();

		inline QIODevice* device() const { return _device; }
		void setDevice(QIODevice* device);

	public:
		void get(const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());
		void head(const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());
		void post(const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());
		void put(const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());
		void deleteResource(const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());

		void write(const QByteArray& method, const QByteArray& path, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());

	private:
		QIODevice* _device;
		QByteArray _builder;
	};

	//
	// Pillow::HttpResponseParser
	//
	// A low-level parser for HTTP/1.1 responses. Built on joyent's http_parser.
	//
	// Reentrant. Not thread safe.
	//
	class PILLOWCORE_EXPORT HttpResponseParser
	{
	public:
		HttpResponseParser();
		virtual ~HttpResponseParser();

	public:
		// The "int inject" methods return the number of bytes that were consumed.
		int inject(const char* data, int length);
		inline int inject(const QByteArray& data) { return inject(data.constBegin(), data.size()); }

		// Let the parser know that the connection/device/source has closed and will not be providing more data.
		void injectEof();

		// Clear the parser back to the initial state, ready to parse a new message.
		void clear();

	public:
		// Parser status members.
		inline bool isParsing() const { return _parsing; }
		inline bool hasError() const { return !(parser.http_errno == HPE_OK || parser.http_errno == HPE_PAUSED); }
		inline http_errno error() const { return static_cast<http_errno>(parser.http_errno); }
		QByteArray errorString() const;

		// Response members.
		inline quint16 statusCode() const { return parser.status_code; }
		inline quint16 httpMajor() const { return parser.http_major; }
		inline quint16 httpMinor() const { return parser.http_minor; }
		inline const Pillow::HttpHeaderCollection& headers() const { return _headers; }
		inline const QByteArray& content() const { return _content; }

		// Returns whether the http client should reuse the same communication channel at the end of the current request or close it.
		inline bool shouldKeepAlive() const { return http_should_keep_alive(const_cast<http_parser*>(&parser)); }

		// Returns whether the parser needs EOF to know where the message completes. The http client should use the injectEof() method to let the parser know that EOF was encountered.
		inline bool completesOnEof() const { return http_message_needs_eof(const_cast<http_parser*>(&parser)); }

	protected:
		// Parser callbacks.
		virtual void messageBegin();    // Defaults to clearing previous response members.
		virtual void headersComplete(); // Defaults to doing nothing.
		virtual void messageContent(const char *data, int length); // Defaults to appending to _content
		virtual void messageComplete(); // Defaults to doing nothing.

		// Pause the parser at the current parsing position and make it return from inject(). Only valid if called from within one of the callbacks above.
		void pause();

	private:
		static int parser_on_message_begin(http_parser* parser);
		static int parser_on_header_field(http_parser* parser, const char *at, size_t length);
		static int parser_on_header_value(http_parser* parser, const char *at, size_t length);
		static int parser_on_headers_complete(http_parser* parser);
		static int parser_on_body(http_parser* parser, const char *at, size_t length);
		static int parser_on_message_complete(http_parser* parser);

	private:
		void pushHeader();

	private:
		http_parser parser;
		http_parser_settings parser_settings;
		QByteArray _field, _value;
		bool _lastWasValue;
		bool _parsing; // Whether the parser is currently parsing data (will be true in callbacks).

	protected:
		Pillow::HttpHeaderCollection _headers;
		QByteArray _content;
	};

	//
	// Pillow::HttpClientRequest
	//
	// A small utility class to represent an http request.
	//
	struct PILLOWCORE_EXPORT HttpClientRequest
	{
		QByteArray method;
		QUrl url;
		Pillow::HttpHeaderCollection headers;
		QByteArray data;
	};

	//
	// Pillow::HttpClient
	//
	// Small, fast and simple low-level HTTP/1.1 client.
	//
	// Reentrant. Not thread safe.
	//
	class PILLOWCORE_EXPORT HttpClient : public QObject, protected HttpResponseParser
	{
		Q_OBJECT
		Q_FLAGS(Error)

	public:
		enum Error
		{
			NoError,				// There was no error in sending and receiving the previous request (if any),
									// including the 4xx and 5xx responses with represent client and server errors.

			NetworkError,			// There was a network error (unable to connect, could not resolve host, etc.).
			ResponseInvalidError,	// The response from the server could not be parsed as a valid Http response.
			RemoteHostClosedError,  // The remote server has closed the connection before sending a full reply.
			AbortedError            // The request was aborted before a pending response was completed.
		};

	public:
		HttpClient(QObject* parent = 0);

		// keepAliveTimeout: Maximum time, in milliseconds, for which the client will keep an established connection channel
		//                   between requests to the same host. Use 0 to disable keep-alive.
		//                   Defaults to -1 (no timeout).
		int keepAliveTimeout() const;
		void setKeepAliveTimeout(int timeout);

		// readBufferSize: Maximum size, in bytes, of network read buffers. When the buffers are full, TCP congestion
		//                 control will occur. Use consumeContent() to consume data in the buffers and make room for more
		//                 data. Due to an implementation detail, the effective amount of buffered data can be higher
		//                 than readBufferSize.
		//                 Defaults to 0 (unlimited).
		qint64 readBufferSize() const;
		void setReadBufferSize(qint64 size);

	public:
		// Request members.
		void get(const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());
		void head(const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());
		void post(const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());
		void put(const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());
		void deleteResource(const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection());

		void request(const QByteArray& method, const QUrl& url, const Pillow::HttpHeaderCollection& headers = Pillow::HttpHeaderCollection(), const QByteArray& data = QByteArray());
		void request(const Pillow::HttpClientRequest& request);

		void abort(); // Stop any active request and break current server connection. If there was an active request, finished() will be emitted and the error will be set to AbortedError.

		void followRedirection(); // Follow previous request's redirection. Only effective if redirected() is true.

	public:
		// Response Members.
		bool responsePending() const;
		Error error() const;

		inline int statusCode() const { return static_cast<int>(HttpResponseParser::statusCode()); }
		inline const Pillow::HttpHeaderCollection& headers() const { return HttpResponseParser::headers(); }
		inline const QByteArray& content() const { return HttpResponseParser::content(); }

		QByteArray consumeContent(); // Get the value of "content()" and clear the internal buffer.

		bool redirected() const;
		QByteArray redirectionLocation() const;

	signals:
		void headersCompleted(); // Headers have been fully received and are ready to be checked.
		void contentReadyRead(); // Some new data is available in the response content.
		void finished();         // Request finished. Check error() to verify if there was an error.

	private slots:
		void device_error(QAbstractSocket::SocketError error);
		void device_connected();
		void device_readyRead();

	private:
		void sendRequest();

	protected:
		void messageBegin();
		void headersComplete();
		void messageContent(const char *data, int length);
		void messageComplete();

	private:
		QTcpSocket* _device;
		Pillow::HttpClientRequest _request;
		Pillow::HttpClientRequest _pendingRequest;
		Pillow::HttpRequestWriter _requestWriter;
		Pillow::HttpResponseParser _responseParser;
		bool _responsePending;
		Error _error;
		QByteArray _buffer;
		int _keepAliveTimeout;
		QElapsedTimer _keepAliveTimeoutTimer;
		QByteArray _hostHeaderValue;
		Pillow::ContentTransformer* _contentDecoder;
	};

	//
	// Pillow::NetworkAcccessManager
	//
	// A QNetworkAccessManager that uses Pillow::HttpClient for http. Provides unlimited connections
	// per host for http instead of the 6 connections-per-host limit that QNetworkAccessManager normally
	// applies.
	//
	// Reentrant. Not thread safe.
	//
	class PILLOWCORE_EXPORT NetworkAccessManager : public QNetworkAccessManager
	{
		Q_OBJECT

	public:
		NetworkAccessManager(QObject *parent = 0);
		~NetworkAccessManager();

	protected:
		QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData = 0);

	private slots:
		void client_finished();

	private:
		typedef QMultiHash<QString, Pillow::HttpClient*> UrlClientsMap;
		UrlClientsMap _urlToClientsMap;
		typedef QHash<Pillow::HttpClient*, QString> ClientUrlMap;
		ClientUrlMap _clientToUrlMap;
	};

} // namespace Pillow

#endif // PILLOW_HTTPCLIENT_H
