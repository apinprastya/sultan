#include "HttpClient.h"
#include "ByteArrayHelpers.h"
#include <QtCore/QIODevice>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QTimer>
#include "private/zlib.h"

namespace Pillow
{
	namespace HttpClientTokens
	{
		const QByteArray getMethodToken("GET");
		const QByteArray headMethodToken("HEAD");
		const QByteArray postMethodToken("POST");
		const QByteArray putMethodToken("PUT");
		const QByteArray deleteMethodToken("DELETE");
		const QByteArray crlfToken("\r\n");
		const QByteArray colonSpaceToken(": ");
		const QByteArray httpOneOneCrlfToken(" HTTP/1.1\r\n");
		const QByteArray contentLengthColonSpaceToken("Content-Length: ");
		const QByteArray hostToken("Host");
	}

	class ContentTransformer
	{
	public:
		virtual ~ContentTransformer() {}
		virtual QByteArray transform(const char *data, int length) = 0;
	};

#ifdef PILLOW_ZLIB
	class GunzipContentTransformer : public ContentTransformer
	{
	public:
		GunzipContentTransformer(): _streamBad(false)
		{
			memset(&_inflateStream, 0, sizeof(z_stream));
			inflateInit2(&_inflateStream, 31);
		}

		~GunzipContentTransformer()
		{
			inflateEnd(&_inflateStream);
		}

		QByteArray transform(const char *data, int length)
		{
			unsigned char buffer[32 * 1024];
            if (_inflatedBuffer.isDetached())
                _inflatedBuffer.data_ptr()->size = 0;
            else
                _inflatedBuffer.clear();

			_inflateStream.next_in = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data));
			_inflateStream.avail_in = length;

			do
			{
				_inflateStream.next_out = buffer;
				_inflateStream.avail_out = sizeof(buffer);

				if (_streamBad || inflate(&_inflateStream, Z_NO_FLUSH) < 0) // Less than 0 is error.
				{
					qWarning("Pillow::GunzipContentTransformer::transform: error inflating input stream passing original content through.");
					_streamBad = true;
					break;
				}

				_inflatedBuffer.append(reinterpret_cast<const char*>(buffer), sizeof(buffer) - _inflateStream.avail_out);
			}
			while (_inflateStream.avail_in > 0 && _inflateStream.avail_out == 0);

			if (_streamBad)
			{
                if (_inflatedBuffer.isDetached())
                    _inflatedBuffer.data_ptr()->size = 0;
                else
                    _inflatedBuffer.clear();
                _inflatedBuffer.append(data, length);
			}

			return _inflatedBuffer;
		}

	private:
		z_stream _inflateStream;
		QByteArray _inflatedBuffer;
		bool _streamBad;
	};
#endif // PILLOW_ZLIB
}

//
// Pillow::HttpRequestWriter
//

Pillow::HttpRequestWriter::HttpRequestWriter()
	: _device(0)
{
}

void Pillow::HttpRequestWriter::get(const QByteArray &path, const Pillow::HttpHeaderCollection &headers)
{
	write(Pillow::HttpClientTokens::getMethodToken, path, headers);
}

void Pillow::HttpRequestWriter::head(const QByteArray &path, const Pillow::HttpHeaderCollection &headers)
{
	write(Pillow::HttpClientTokens::headMethodToken, path, headers);
}

void Pillow::HttpRequestWriter::post(const QByteArray &path, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	write(Pillow::HttpClientTokens::postMethodToken, path, headers, data);
}

void Pillow::HttpRequestWriter::put(const QByteArray &path, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	write(Pillow::HttpClientTokens::putMethodToken, path, headers, data);
}

void Pillow::HttpRequestWriter::deleteResource(const QByteArray &path, const Pillow::HttpHeaderCollection &headers)
{
	write(Pillow::HttpClientTokens::deleteMethodToken, path, headers);
}

void Pillow::HttpRequestWriter::write(const QByteArray &method, const QByteArray &path, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	if (_device == 0)
	{
		qWarning() << "Pillow::HttpRequestWriter::write: called while device is not set. Not proceeding.";
		return;
	}

	if (_builder.capacity() < 8192)
		_builder.reserve(8192);

	_builder.append(method).append(' ').append(path).append(Pillow::HttpClientTokens::httpOneOneCrlfToken);

	for (const Pillow::HttpHeader *h = headers.constBegin(), *hE = headers.constEnd(); h < hE; ++h)
		_builder.append(h->first).append(Pillow::HttpClientTokens::colonSpaceToken).append(h->second).append(Pillow::HttpClientTokens::crlfToken);

	if (!data.isEmpty())
	{
		_builder.append(Pillow::HttpClientTokens::contentLengthColonSpaceToken);
		Pillow::ByteArrayHelpers::appendNumber<int, 10>(_builder, data.size());
		_builder.append(Pillow::HttpClientTokens::crlfToken);
	}

	_builder.append(Pillow::HttpClientTokens::crlfToken);

	if (data.isEmpty())
	{
		_device->write(_builder);
	}
	else
	{
		if (data.size() < 4096)
		{
			_builder.append(data);
			_device->write(_builder);
		}
		else
		{
			_device->write(_builder);
			_device->write(data);
		}
	}

	if (_builder.size() > 16384)
		_builder.clear();
	else
		_builder.data_ptr()->size = 0;
}

void Pillow::HttpRequestWriter::setDevice(QIODevice *device)
{
	if (_device == device) return;
	_device = device;
}

//
// Pillow::HttpResponseParser
//

Pillow::HttpResponseParser::HttpResponseParser()
	: _lastWasValue(false), _parsing(false)
{
	parser.data = this;
	parser_settings.on_message_begin = parser_on_message_begin;
	parser_settings.on_header_field = parser_on_header_field;
	parser_settings.on_header_value = parser_on_header_value;
	parser_settings.on_headers_complete = parser_on_headers_complete;
	parser_settings.on_body = parser_on_body;
	parser_settings.on_message_complete = parser_on_message_complete;
	clear();
}

Pillow::HttpResponseParser::~HttpResponseParser()
{
}

int Pillow::HttpResponseParser::inject(const char *data, int length)
{
	if (_parsing)
	{
		qWarning() << "Pillow::HttpResponseParser::inject: called while parser is parsing. Not proceeding.";
		return 0;
	}

	_parsing = true;
	size_t consumed = http_parser_execute(&parser, &parser_settings, data, length);
	if (parser.http_errno == HPE_PAUSED) parser.http_errno = HPE_OK; // Unpause the parser that got paused upon completing the message.
	_parsing = false;

	return static_cast<int>(consumed);
}

void Pillow::HttpResponseParser::injectEof()
{
	if (_parsing)
	{
		qWarning() << "Pillow::HttpResponseParser::injectEof: called while parser is parsing. Not proceeding.";
		return;
	}

	_parsing = true;
	http_parser_execute(&parser, &parser_settings, 0, 0);
	if (parser.http_errno == HPE_PAUSED) parser.http_errno = HPE_OK; // Unpause the parser that got paused upon completing the message.
	_parsing = false;
}

void Pillow::HttpResponseParser::clear()
{
	http_parser_init(&parser, HTTP_RESPONSE);
	while (!_headers.isEmpty()) _headers.pop_back();
	_content.clear();

	if (_parsing) pause(); // clear() got called from a callback. gracefully recover by pausing the parser.
}

QByteArray Pillow::HttpResponseParser::errorString() const
{
	const char* desc = http_errno_description(HTTP_PARSER_ERRNO(&parser));
	return QByteArray::fromRawData(desc, qstrlen(desc));
}

void Pillow::HttpResponseParser::messageBegin()
{
	while (!_headers.isEmpty()) _headers.pop_back();
	_content.clear();
	_lastWasValue = false;
}

void Pillow::HttpResponseParser::headersComplete()
{
}

void Pillow::HttpResponseParser::messageContent(const char *data, int length)
{
	_content.append(data, length);
}

void Pillow::HttpResponseParser::messageComplete()
{
}

void Pillow::HttpResponseParser::pause()
{
	http_parser_pause(&parser, 1);
}

int Pillow::HttpResponseParser::parser_on_message_begin(http_parser *parser)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->messageBegin();
	return 0;
}

int Pillow::HttpResponseParser::parser_on_header_field(http_parser *parser, const char *at, size_t length)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->pushHeader();
	self->_field.append(at, static_cast<int>(length));
	return 0;
}

int Pillow::HttpResponseParser::parser_on_header_value(http_parser *parser, const char *at, size_t length)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->_value.append(at, static_cast<int>(length));
	self->_lastWasValue = true;
	return 0;
}

int Pillow::HttpResponseParser::parser_on_headers_complete(http_parser *parser)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->pushHeader();
	self->headersComplete();
	return 0;
}

int Pillow::HttpResponseParser::parser_on_body(http_parser *parser, const char *at, size_t length)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->messageContent(at, static_cast<int>(length));
	return 0;
}

int Pillow::HttpResponseParser::parser_on_message_complete(http_parser *parser)
{
	Pillow::HttpResponseParser* self = reinterpret_cast<Pillow::HttpResponseParser*>(parser->data);
	self->messageComplete();

	// Pause the parser to avoid parsing the next message (if there is another one in the buffer).
	http_parser_pause(parser, 1);

	return 0;
}

inline void Pillow::HttpResponseParser::pushHeader()
{
	if (_lastWasValue)
	{
		_headers << Pillow::HttpHeader(_field, _value);
		_field.clear();
		_value.clear();
		_lastWasValue = false;
	}
}

//
// Pillow::HttpClient
//

Pillow::HttpClient::HttpClient(QObject *parent)
	: QObject(parent), _responsePending(false), _error(NoError), _keepAliveTimeout(-1), _contentDecoder(0)
{
	_device = new QTcpSocket(this);
	connect(_device, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(device_error(QAbstractSocket::SocketError)));
	connect(_device, SIGNAL(connected()), this, SLOT(device_connected()));
	connect(_device, SIGNAL(readyRead()), this, SLOT(device_readyRead()));
	_requestWriter.setDevice(_device);
	_keepAliveTimeoutTimer.invalidate();
}

int Pillow::HttpClient::keepAliveTimeout() const
{
	return _keepAliveTimeout;
}

void Pillow::HttpClient::setKeepAliveTimeout(int timeout)
{
	_keepAliveTimeout = timeout;
}

qint64 Pillow::HttpClient::readBufferSize() const
{
	return _device->readBufferSize();
}

void Pillow::HttpClient::setReadBufferSize(qint64 size)
{
	_device->setReadBufferSize(size);
}

bool Pillow::HttpClient::responsePending() const
{
	return _responsePending;
}

Pillow::HttpClient::Error Pillow::HttpClient::error() const
{
	return _error;
}

bool Pillow::HttpClient::redirected() const
{
	switch (statusCode())
	{
	case 300: // Multiple Choices
	case 301: // Moved Permanently
	case 302: // Found
	case 303: // See Other
	case 307: // Temporary Redirect
		return true;

	default:
		return false;
	}
}

QByteArray Pillow::HttpClient::redirectionLocation() const
{
	foreach (const Pillow::HttpHeader &header, headers())
	{
		if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(header.first, Pillow::LowerCaseToken("location")))
			return header.second;
	}
	return QByteArray();
}

QByteArray Pillow::HttpClient::consumeContent()
{
	QByteArray c = _content;
	_content = QByteArray();

	if (responsePending() && _device->bytesAvailable())
		QTimer::singleShot(0, this, SLOT(device_readyRead()));

	return c;
}

void Pillow::HttpClient::get(const QUrl &url, const Pillow::HttpHeaderCollection &headers)
{
	request(Pillow::HttpClientTokens::getMethodToken, url, headers);
}

void Pillow::HttpClient::head(const QUrl &url, const Pillow::HttpHeaderCollection &headers)
{
	request(Pillow::HttpClientTokens::headMethodToken, url, headers);
}

void Pillow::HttpClient::post(const QUrl &url, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	request(Pillow::HttpClientTokens::postMethodToken, url, headers, data);
}

void Pillow::HttpClient::put(const QUrl &url, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	request(Pillow::HttpClientTokens::putMethodToken, url, headers, data);
}

void Pillow::HttpClient::deleteResource(const QUrl &url, const Pillow::HttpHeaderCollection &headers)
{
	request(Pillow::HttpClientTokens::deleteMethodToken, url, headers);
}

void Pillow::HttpClient::request(const QByteArray &method, const QUrl &url, const Pillow::HttpHeaderCollection &headers, const QByteArray &data)
{
	Pillow::HttpClientRequest newRequest;
	newRequest.method = method;
	newRequest.url = url;
	newRequest.headers = headers;
	newRequest.data = data;
	request(newRequest);
}

void Pillow::HttpClient::request(const Pillow::HttpClientRequest &request)
{
	if (_responsePending)
	{
		qWarning("Pillow::HttpClient::request: cannot send new request while another one is under way. Request pipelining is not supported.");
		return;
	}

	if (Pillow::HttpResponseParser::isParsing())
	{
		// We most likely got cancelled in a callback or got asked for a new request
		// in the messageComplete() callback. So we are not responsePending but still parsing.
		// Keep the request for after we come out of parsing.
		_pendingRequest = request;
		return;
	}

	// We can reuse an active connection if the request is for the same host and port, so make note of those parameters before they are overwritten.
	const QString previousHost = _request.url.host();
	const int previousPort = _request.url.port();

	_request = request;
	_responsePending = true;
	_error = NoError;
	clear();

	const bool isConnected = _device->state() == QAbstractSocket::ConnectedState;
	const bool sameServer = _request.url.host() == previousHost && _request.url.port() == previousPort;
	const bool keepAliveTimeoutExpired = sameServer && (_keepAliveTimeout >= 0 && _keepAliveTimeoutTimer.isValid() && _keepAliveTimeoutTimer.hasExpired(_keepAliveTimeout));
	const bool reuseExistingConnection = isConnected && sameServer && !keepAliveTimeoutExpired;

	if (reuseExistingConnection)
	{
		// Reuse current connection to same host and port.
		sendRequest();
	}
	else
	{
        if (_hostHeaderValue.isDetached())
            _hostHeaderValue.data_ptr()->size = 0; // Clear the previous host header value (if any), without deallocating memory.
        else
            _hostHeaderValue.clear();

		if (_device->state() != QAbstractSocket::UnconnectedState)
			_device->disconnectFromHost();
		_device->connectToHost(_request.url.host(), _request.url.port(80));
	}
}

void Pillow::HttpClient::abort()
{
	if (_device) _device->abort();

	if (_responsePending)
	{
		Pillow::HttpResponseParser::pause();
		_error = AbortedError;
		_responsePending = false;
		emit finished();
	}
}

void Pillow::HttpClient::followRedirection()
{
	if (!redirected())
	{
		qWarning("Pillow::HttpClient::followRedirection(): no redirection to follow.");
		return;
	}

	Pillow::HttpClientRequest newRequest = _request;
	newRequest.url = QUrl::fromEncoded(redirectionLocation());
	request(newRequest);
}

void Pillow::HttpClient::device_error(QAbstractSocket::SocketError error)
{
	if (!_responsePending)
	{
		// Errors that happen while we are not waiting for a response are ok. We'll try to
		// recover on the next time we get a request.
		return;
	}

	switch (error)
	{
	case QAbstractSocket::RemoteHostClosedError:
		_error = RemoteHostClosedError;
		break;
	default:
		_error = NetworkError;
	}

	_device->close();
	_responsePending = false;
	emit finished();
}

void Pillow::HttpClient::device_connected()
{
	sendRequest();
}

void Pillow::HttpClient::device_readyRead()
{
	if (!responsePending())
	{
		// Not supposed to be receiving data at this point. Just
		// ignore it and close the connection.
		_device->close();
		return;
	}

	qint64 bytesAvailable = _device->bytesAvailable();
	if (bytesAvailable == 0) return;

	if (_buffer.capacity() < _buffer.size() + bytesAvailable)
		_buffer.reserve(_buffer.size() + bytesAvailable);

	qint64 bufferSize = readBufferSize();
	if (bufferSize > 0)
		bytesAvailable = bufferSize - _content.size();

	if (bytesAvailable <= 0)
	{
		// Nothing to read or no space in buffers. Wait for more data or more space in buffers.
		return;
	}

	qint64 bytesRead = _device->read(_buffer.data() + _buffer.size(), bytesAvailable);
	_buffer.data_ptr()->size += bytesRead;

	int consumed = inject(_buffer);

	if (responsePending())
	{
		// Response is still pending. One of the following:
		// 1. Got a parser error. (where hasError)
		// 2. Waiting for more data to complete the current request. (where _pendingRequest is null and consumed == buffer.size)
		// 3. Waiting for the real response after a 100-continue response. (where _pendingRequest is null and consumed < buffer.size, because parser will stop consuming after the 100-continue).

		if (!hasError())
		{
			if (consumed < _buffer.size())
			{
				// We had multiple responses in the buffer?
				// It was a 100 Continue since we are still response pending.
				consumed += inject(_buffer.constData() + consumed, _buffer.size() - consumed);

				if (consumed < _buffer.size())
				{
					qWarning() << "Left some unconsumed data in the buffer:" << (_buffer.size() - consumed);
				}
			}
			else
			{
				// Just waiting for more data to consume.
			}
		}

		if (hasError()) // Re-check for error in case we injected again in the 100 Continue case above.
		{
			_error = ResponseInvalidError;
			_device->close();
			_responsePending = false;
			emit finished();
		}
	}

	// Reuse the read buffer if it is not overly large.
	if (_buffer.capacity() > 128 * 1024)
		_buffer.clear();
	else
		_buffer.data_ptr()->size = 0;

	// Response completed or got aborted in a callback.
	if (!responsePending() && !_pendingRequest.method.isNull())
	{
		// Plus we got a new request while in callback.
		Pillow::HttpClientRequest r = _pendingRequest;
		_pendingRequest = Pillow::HttpClientRequest(); // Clear it.
		request(r);
	}

}

void Pillow::HttpClient::sendRequest()
{
	if (!responsePending()) return;

	if (_hostHeaderValue.isEmpty())
	{
        _hostHeaderValue = _request.url.encodedHost();
		if (_request.url.port(80) != 80)
		{
			_hostHeaderValue.append(':');
			Pillow::ByteArrayHelpers::appendNumber<int, 10>(_hostHeaderValue, _request.url.port(80));
		}
	}

	QByteArray uri = _request.url.encodedPath();
	const QByteArray query = _request.url.encodedQuery();
	if (!query.isEmpty()) uri.append('?').append(query);

	Pillow::HttpHeaderCollection headers;
	headers.reserve(_request.headers.size() + 1);
	headers << Pillow::HttpHeader(Pillow::HttpClientTokens::hostToken, _hostHeaderValue);
	for (int i = 0, iE = _request.headers.size(); i < iE; ++i)
		headers << _request.headers.at(i);

	_requestWriter.write(_request.method, uri, headers, _request.data);
}

void Pillow::HttpClient::messageBegin()
{
	Pillow::HttpResponseParser::messageBegin();
}

void Pillow::HttpClient::headersComplete()
{
	Pillow::HttpResponseParser::headersComplete();
	if (statusCode() == 100)
		return;

	emit headersCompleted();

	if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(_request.method, Pillow::LowerCaseToken("head")))
	{
		pause();
		messageComplete();
	}
	else
	{
#ifdef PILLOW_ZLIB
		for (int i = 0, iE = _headers.size(); i < iE; ++i)
		{
			const Pillow::HttpHeader& header = _headers.at(i);
			if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(header.first, Pillow::LowerCaseToken("content-encoding")))
			{
				if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(header.second, Pillow::LowerCaseToken("gzip")))
					_contentDecoder = new Pillow::GunzipContentTransformer();
			}
		}
#endif // PILLOW_ZLIB
	}
}

void Pillow::HttpClient::messageContent(const char *data, int length)
{
	if (_contentDecoder)
	{
		const QByteArray decoded = _contentDecoder->transform(data, length);
		Pillow::HttpResponseParser::messageContent(decoded.data(), decoded.length());
	}
	else
	{
		Pillow::HttpResponseParser::messageContent(data, length);
	}
	emit contentReadyRead();
}

void Pillow::HttpClient::messageComplete()
{
	delete _contentDecoder;
	_contentDecoder = 0;

	if (statusCode() == 100)
	{
		// Completely hide and ignore the 100 response we just received.
		clear();
	}
	else
	{
		Pillow::HttpResponseParser::messageComplete();
		_responsePending = false;

		if (_keepAliveTimeout == 0)
			_device->close();
		else
		{
			if (!shouldKeepAlive())
				_device->close();

			_keepAliveTimeoutTimer.start();
		}

		emit finished();
	}
}

//
// Pillow::NetworkReply
//

namespace Pillow
{
	class NetworkReply : public QNetworkReply
	{
		Q_OBJECT

	public:
		NetworkReply(Pillow::HttpClient *client, QNetworkAccessManager::Operation op, const QNetworkRequest& request)
			:_client(client), _contentPos(0)
		{
			connect(client, SIGNAL(headersCompleted()), this, SLOT(client_headersCompleted()));
			connect(client, SIGNAL(contentReadyRead()), this, SLOT(client_contentReadyRead()));
			connect(client, SIGNAL(finished()), this, SLOT(client_finished()));

			setOperation(op);
			setRequest(request);
			setUrl(request.url());
		}

		~NetworkReply()
		{}

	public:
		void abort()
		{
			if (_client) _client->abort();
		}

		qint64 bytesAvailable() const
		{
			return QNetworkReply::bytesAvailable() + _content.size() - _contentPos;
		}

	private slots:
		void client_headersCompleted()
		{
			QList<QNetworkCookie> cookies;

			setAttribute(QNetworkRequest::HttpStatusCodeAttribute, _client->statusCode());

			// Setting the reason phrase is not done as the http_parser does not give us access
			// to the real reason phrase returned by the server.
			//
			// setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, ...);

			foreach (const Pillow::HttpHeader &header, _client->headers())
			{
				setRawHeader(header.first, header.second);

				if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(header.first, Pillow::LowerCaseToken("set-cookie")))
					cookies += QNetworkCookie::parseCookies(header.second);
				else if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(header.first, Pillow::LowerCaseToken("location")))
					setAttribute(QNetworkRequest::RedirectionTargetAttribute, QUrl::fromEncoded(header.second));
			}

			if (!cookies.isEmpty())
				setHeader(QNetworkRequest::SetCookieHeader, QVariant::fromValue(cookies));

			QNetworkAccessManager *nam = manager();
			if (nam)
			{
				QNetworkCookieJar *jar = nam->cookieJar();
				if (jar)
				{
					QUrl url = request().url();
					url.setPath("/");
					jar->setCookiesFromUrl(cookies, url);
				}
			}

			open(QIODevice::ReadOnly | QIODevice::Unbuffered);

			emit metaDataChanged();
		}

		void client_contentReadyRead()
		{
			_content.append(_client->consumeContent());
			 emit readyRead();
			 //emit downloadProgress();
		}

		void client_finished()
		{
			disconnect(_client, 0, this, 0);

			if (_client->error() != Pillow::HttpClient::NoError)
			{
				QNetworkReply::NetworkError error = QNetworkReply::UnknownNetworkError;

				switch (_client->error())
				{
				case Pillow::HttpClient::NoError: error = QNetworkReply::NoError; break;
				case Pillow::HttpClient::NetworkError: error = QNetworkReply::UnknownNetworkError; break;
				case Pillow::HttpClient::ResponseInvalidError: error = QNetworkReply::ProtocolUnknownError; break;
				case Pillow::HttpClient::RemoteHostClosedError: error = QNetworkReply::RemoteHostClosedError; break;
				case Pillow::HttpClient::AbortedError: error = QNetworkReply::OperationCanceledError; break;
				}

				emit this->error(error);
			}

			_client = 0;
			setFinished(true);
			emit finished();
		}

	protected:
		qint64 readData(char *data, qint64 maxSize)
		{
			if (_contentPos >= _content.size()) return -1;
			qint64 bytesRead = qMin(maxSize, static_cast<qint64>(_content.size() - _contentPos));
			memcpy(data, _content.constData() + _contentPos, bytesRead);
			_contentPos += bytesRead;
			return bytesRead;
		}

	private:
		Pillow::HttpClient *_client;
		QByteArray _content;
		int _contentPos;
	};
}

//
// Pillow::NetworkAccessManager
//

Pillow::NetworkAccessManager::NetworkAccessManager(QObject *parent)
	: QNetworkAccessManager(parent)
{
}

Pillow::NetworkAccessManager::~NetworkAccessManager()
{
}

QNetworkReply *Pillow::NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	if (request.url().scheme().compare(QLatin1String("http"), Qt::CaseInsensitive) != 0)
	{
		// Use base implementation for unsupported schemes.
		return QNetworkAccessManager::createRequest(op, request, outgoingData);
	}

	const QString urlAuthority = request.url().authority();

	UrlClientsMap::Iterator it = _urlToClientsMap.find(urlAuthority);
	Pillow::HttpClient *client = 0;
	if (it != _urlToClientsMap.end())
	{
		client = it.value();
		_urlToClientsMap.erase(it);
	}
	if (client == 0)
	{
		client = new Pillow::HttpClient(this);
		_clientToUrlMap.insert(client, urlAuthority);
		connect(client, SIGNAL(finished()), this, SLOT(client_finished()), Qt::DirectConnection);
	}

	Pillow::NetworkReply *reply = new Pillow::NetworkReply(client, op, request);

	Pillow::HttpHeaderCollection headers;
	foreach (const QByteArray &headerName, request.rawHeaderList())
		headers << Pillow::HttpHeader(headerName, request.rawHeader(headerName));

//	headers << Pillow::HttpHeader("Accept-Encoding", "gzip");

	QNetworkCookieJar *jar = cookieJar();
	if (jar)
	{
		const QList<QNetworkCookie> cookies = jar->cookiesForUrl(request.url());
		QByteArray cookieHeaderValue;
		for (int i = 0, iE = cookies.size(); i < iE; ++i)
		{
			if (i > 0) cookieHeaderValue.append("; ");
			cookieHeaderValue.append(cookies.at(i).toRawForm(QNetworkCookie::NameAndValueOnly));
		}
		if (!cookieHeaderValue.isEmpty())
			headers << Pillow::HttpHeader("Cookie", cookieHeaderValue);
	}

	switch (op)
	{
	case QNetworkAccessManager::HeadOperation:
		client->head(request.url(), headers);
		break;
	case QNetworkAccessManager::GetOperation:
		client->get(request.url(), headers);
		break;
	case QNetworkAccessManager::PutOperation:
		client->put(request.url(), headers, outgoingData ? outgoingData->readAll() : QByteArray());
		break;
	case QNetworkAccessManager::PostOperation:
		client->post(request.url(), headers, outgoingData ? outgoingData->readAll() : QByteArray());
		break;
	case QNetworkAccessManager::DeleteOperation:
		client->deleteResource(request.url(), headers);
		break;
	case QNetworkAccessManager::CustomOperation:
		client->request(request.attribute(QNetworkRequest::CustomVerbAttribute).toByteArray(), request.url(), headers, outgoingData ? outgoingData->readAll() : QByteArray());
		break;

	case QNetworkAccessManager::UnknownOperation:
		break;
	}

	return reply;
}

void Pillow::NetworkAccessManager::client_finished()
{
	Pillow::HttpClient *client = static_cast<Pillow::HttpClient*>(sender());
	const QString urlAuthority = _clientToUrlMap.value(client);

	if (urlAuthority.isEmpty())
		client->deleteLater();
	else
		_urlToClientsMap.insert(urlAuthority, client);
}

#include "HttpClient.moc"
