#include "HttpHandler.h"
#include "HttpConnection.h"
#include "HttpHelpers.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QDateTime>
#include <QtCore/QStringBuilder>
#include <QtNetwork/QTcpSocket>
using namespace Pillow;

//
// HttpHandler
//

HttpHandler::HttpHandler(QObject *parent)
	: QObject(parent)
{
}

//
// HttpHandlerStack
//

HttpHandlerStack::HttpHandlerStack(QObject *parent)
	: HttpHandler(parent)
{
}

bool HttpHandlerStack::handleRequest(Pillow::HttpConnection *connection)
{
	foreach (QObject* object, children())
	{
		HttpHandler* handler = qobject_cast<HttpHandler*>(object);

		if (handler && handler->handleRequest(connection))
			return true;
	}

	return false;
}

//
// HttpHandlerFixed
//

HttpHandlerFixed::HttpHandlerFixed(int statusCode, const QByteArray& content, QObject *parent)
	:HttpHandler(parent), _statusCode(statusCode), _content(content)
{
}

void Pillow::HttpHandlerFixed::setStatusCode(int statusCode)
{
	if (_statusCode == statusCode) return;
	_statusCode = statusCode;
	emit changed();
}

void Pillow::HttpHandlerFixed::setContent(const QByteArray &content)
{
	if (_content == content) return;
	_content = content;
	emit changed();
}

bool HttpHandlerFixed::handleRequest(Pillow::HttpConnection *connection)
{
	connection->writeResponse(_statusCode, HttpHeaderCollection(), _content);
	return true;
}

//
// HttpHandler404
//

HttpHandler404::HttpHandler404(QObject *parent)
	: HttpHandler(parent)
{
}

bool HttpHandler404::handleRequest(Pillow::HttpConnection *connection)
{
	connection->writeResponseString(404, HttpHeaderCollection(), QString("The requested resource '%1' does not exist on this server").arg(QString(connection->requestPath())));
	return true;
}

#ifdef Q_COMPILER_LAMBDA
//
// HttpHandlerFunction
//

HttpHandlerFunction::HttpHandlerFunction(QObject *parent)
	:HttpHandler(parent), _function()
{
}

HttpHandlerFunction::HttpHandlerFunction(const std::function<void (HttpConnection *)> &function, QObject *parent)
	:HttpHandler(parent), _function(function)
{
}

bool HttpHandlerFunction::handleRequest(HttpConnection *connection)
{
	if (_function)
	{
		_function(connection);
		return true;
	}
	else
		return false;
}
#endif // Q_COMPILER_LAMBDA

//
// HttpHandlerLog
//

HttpHandlerLog::HttpHandlerLog(QObject *parent)
	: HttpHandler(parent), _mode(LogCompletedRequests), _device(0)
{
}

HttpHandlerLog::HttpHandlerLog(HttpHandlerLog::Mode mode, QIODevice *device, QObject *parent)
	: HttpHandler(parent), _mode(mode), _device(device)
{
}

HttpHandlerLog::HttpHandlerLog(QIODevice *device, QObject *parent)
	: HttpHandler(parent), _mode(LogCompletedRequests), _device(device)
{
}

HttpHandlerLog::~HttpHandlerLog()
{
	foreach (QElapsedTimer* timer, _requestTimerMap)
		delete timer;
}

bool HttpHandlerLog::handleRequest(Pillow::HttpConnection *connection)
{
	QElapsedTimer* timer = _requestTimerMap.value(connection, NULL);
	if (timer == NULL)
	{
		timer = _requestTimerMap[connection] = new QElapsedTimer();
		connect(connection, SIGNAL(requestCompleted(Pillow::HttpConnection*)), this, SLOT(requestCompleted(Pillow::HttpConnection*)));
		connect(connection, SIGNAL(closed(Pillow::HttpConnection*)), this, SLOT(requestClosed(Pillow::HttpConnection*)));
		connect(connection, SIGNAL(destroyed(QObject*)), this, SLOT(requestDestroyed(QObject*)));
	}
	timer->start();

	if (_mode == LogCompletedRequests)
	{
		// Nothing to log here.
	}
	else if (_mode == TraceRequests)
	{
		QString logEntry = QString("[BEGIN] %1 - - [%2] \"%3 %4 %5\" - - -")
				.arg(connection->remoteAddress().toString())
				.arg(QDateTime::currentDateTime().toString("dd/MMM/yyyy hh:mm:ss"))
				.arg(QString(connection->requestMethod())).arg(QString(connection->requestUri())).arg(QString(connection->requestHttpVersion()));

		log(logEntry);
	}

	return false;
}

void HttpHandlerLog::requestCompleted(Pillow::HttpConnection *connection)
{
	QElapsedTimer* timer = _requestTimerMap.value(connection, NULL);
	if (timer)
	{
		const char* formatString = (_mode == LogCompletedRequests) ? "%1 - - [%2] \"%3 %4 %5\" %6 %7 %8" : "[ END ] %1 - - [%2] \"%3 %4 %5\" %6 %7 %8";

		qint64 elapsed = timer->elapsed();
		QString logEntry = QString(formatString)
				.arg(connection->remoteAddress().toString())
				.arg(QDateTime::currentDateTime().toString("dd/MMM/yyyy hh:mm:ss"))
				.arg(QString(connection->requestMethod())).arg(QString(connection->requestUri())).arg(QString(connection->requestHttpVersion()))
				.arg(connection->responseStatusCode()).arg(connection->responseContentLength())
				.arg(elapsed / 1000.0, 3, 'f', 3);

		log(logEntry);
	}
}

void HttpHandlerLog::requestClosed(HttpConnection *connection)
{
	QElapsedTimer* timer = _requestTimerMap.value(connection, NULL);
	if (timer && _mode == TraceRequests)
	{
		const char* formatString = "[CLOSE] %1 - - [%2] \"%3 %4 %5\" %6 %7 %8";

		qint64 elapsed = timer->elapsed();
		QString logEntry = QString(formatString)
				.arg(connection->remoteAddress().toString())
				.arg(QDateTime::currentDateTime().toString("dd/MMM/yyyy hh:mm:ss"))
				.arg(QString(connection->requestMethod())).arg(QString(connection->requestUri())).arg(QString(connection->requestHttpVersion()))
				.arg(connection->responseStatusCode()).arg(connection->responseContentLength())
				.arg(elapsed / 1000.0, 3, 'f', 3);

		log(logEntry);
	}
}

void HttpHandlerLog::requestDestroyed(QObject *r)
{
	HttpConnection* connection = static_cast<HttpConnection*>(r);
	delete _requestTimerMap.value(connection, NULL);
	_requestTimerMap.remove(connection);
}

void HttpHandlerLog::log(const QString &entry)
{
	if (_device == NULL)
		qDebug() << qPrintable(entry);
	else
	{
		_device->write(entry.toUtf8());
		_device->write("\n", 1);
	}
}

QIODevice * HttpHandlerLog::device() const
{
	return _device;
}

void HttpHandlerLog::setMode(HttpHandlerLog::Mode mode)
{
	if (_mode == mode) return;
	_mode = mode;
}

void Pillow::HttpHandlerLog::setDevice(QIODevice *device)
{
	if (_device == device) return;
	_device = device;
}

//
// HttpHandlerFile
//

HttpHandlerFile::HttpHandlerFile(const QString &publicPath, QObject *parent)
	: HttpHandler(parent), _bufferSize(DefaultBufferSize)
{
	setPublicPath(publicPath);
}

void HttpHandlerFile::setPublicPath(const QString &publicPath)
{
	if (_publicPath == publicPath) return;
	_publicPath = publicPath;

	if (!_publicPath.isEmpty())
	{
		QFileInfo pathInfo(_publicPath);
		_publicPath = pathInfo.canonicalFilePath();
		if (!(pathInfo.exists() && pathInfo.isDir() && pathInfo.isReadable()))
		{
			qWarning() << "HttpHandlerStaticFile::SetPublicPath:" << publicPath << "does not exist or is not a readable directory.";
		}
	}
	emit changed();
}

void HttpHandlerFile::setBufferSize(int bytes)
{
	if (_bufferSize == bytes) return;
	_bufferSize = bytes;
}

bool HttpHandlerFile::handleRequest(Pillow::HttpConnection *connection)
{
	if (_publicPath.isEmpty()) { return false; } // Just don't allow access to the root filesystem unless really configured for it.

	QString requestPath = QByteArray::fromPercentEncoding(connection->requestPath());
	QString resultPath = _publicPath + requestPath;
	QFileInfo resultPathInfo(resultPath);

	if (!resultPathInfo.exists())
	{
		return false;
	}
	else if (!resultPathInfo.canonicalFilePath().startsWith(_publicPath))
	{
		return false; // Somebody tried to use some ".." or has followed symlinks that escaped out of the public path.
	}
	else if (!resultPathInfo.isFile())
	{
		return false; // This class does not serve anything else than files... No directory listings!
	}

	QFile* file = new QFile(resultPathInfo.filePath());

	if (!file->open(QIODevice::ReadOnly))
	{
		// Could not read the file?
		connection->writeResponse(403, HttpHeaderCollection(), QString("The requested resource '%1' is not accessible").arg(requestPath).toUtf8());
		delete file;
	}
	else
	{
		HttpHeaderCollection headers; headers.reserve(2);
		headers << HttpHeader("Content-Type", HttpMimeHelper::getMimeTypeForFilename(requestPath));

		if (file->size() <= bufferSize())
		{
			// The file fully fits in the supported buffer size. Read it and calculate an ETag for caching.
			QByteArray content = file->readAll();
			QCryptographicHash md5sum(QCryptographicHash::Md5); md5sum.addData(content);
			QByteArray etag = md5sum.result().toHex();
			headers << HttpHeader("ETag", etag);

			if (connection->requestHeaderValue("If-None-Match") == etag)
				connection->writeResponse(304); // The client's cached file was not modified.
			else
				connection->writeResponse(200, headers, content);

			delete file;
		}
		else
		{
			// The file exceeds the buffer size and must be sent incrementally. Do send the headers right away.
			headers << HttpHeader("Content-Length", QByteArray::number(file->size()));
			connection->writeHeaders(200, headers);

			HttpHandlerFileTransfer* transfer = new HttpHandlerFileTransfer(file, connection, bufferSize());
			file->setParent(transfer);
			//transfer->setParent(this);
			connect(transfer, SIGNAL(finished()), transfer, SLOT(deleteLater()));
			transfer->writeNextPayload();
		}
	}

	return true;
}

//
// HttpHandlerFile
//

HttpHandlerFileTransfer::HttpHandlerFileTransfer(QIODevice *sourceDevice, HttpConnection *connection, int bufferSize)
	: _sourceDevice(sourceDevice), _connection(connection), _bufferSize(bufferSize)
{
	if (bufferSize < 512)
	{
		_bufferSize = 512;
		qWarning() << "HttpHandlerFileTransfer::HttpHandlerFileTransfer: requesting a buffer size of" << bufferSize << "bytes. Correcting to" << _bufferSize << "bytes.";
	}

	connect(sourceDevice, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(_connection, SIGNAL(requestCompleted(Pillow::HttpConnection*)), this, SLOT(deleteLater()));
	connect(_connection, SIGNAL(closed(Pillow::HttpConnection*)), this, SLOT(deleteLater()));
	connect(_connection, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(_connection->outputDevice(), SIGNAL(bytesWritten(qint64)), this, SLOT(writeNextPayload()), Qt::QueuedConnection);
}

void HttpHandlerFileTransfer::writeNextPayload()
{
	if (_sourceDevice == NULL || _connection == NULL || _connection->outputDevice() == NULL) return;

	qint64 bytesToRead = _bufferSize - _connection->outputDevice()->bytesToWrite();
	if (bytesToRead <= 0) return;
	qint64 bytesAvailable = _sourceDevice->size() - _sourceDevice->pos();
	if (bytesToRead > bytesAvailable) bytesToRead = bytesAvailable;

	if (bytesToRead > 0)
	{
		_connection->writeContent(_sourceDevice->read(bytesToRead));

		if (_sourceDevice->atEnd())
			emit finished();
	}
}
