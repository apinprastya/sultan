#ifndef PILLOW_HTTPHANDLER_H
#define PILLOW_HTTPHANDLER_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QOBJECT_H
#include <QtCore/QObject>
#endif // QOBJECT_H
#ifndef QPOINTER_H
#include <QtCore/QPointer>
#endif //QPOINTER_H
#ifndef QHASH_H
#include <QtCore/QHash>
#endif // QHASH_H
#ifdef Q_COMPILER_LAMBDA
#include <functional>
#endif // Q_COMPILER_LAMBDA

class QIODevice;
class QElapsedTimer;

namespace Pillow
{
	class HttpConnection;

	//
	// HttpHandler: abstract handler interface. Does nothing.
	//

	class PILLOWCORE_EXPORT HttpHandler : public QObject
	{
		Q_OBJECT

	public:
		HttpHandler(QObject *parent = 0);

	public slots:
		virtual bool handleRequest(Pillow::HttpConnection* connection) = 0;
	};

	//
	// HttpHandlerStack: a handler that delegates the request to zero or more children handlers until one handles the request.
	//

	class PILLOWCORE_EXPORT HttpHandlerStack : public HttpHandler
	{
		Q_OBJECT

	public:
		HttpHandlerStack(QObject* parent = 0);

	public:
		virtual bool handleRequest(Pillow::HttpConnection *connection);
	};

	//
	// HttpHandlerFixed: a handler that always returns the same specified response.
	//

	class PILLOWCORE_EXPORT HttpHandlerFixed : public HttpHandler
	{
		Q_OBJECT
		Q_PROPERTY(int statusCode READ statusCode WRITE setStatusCode NOTIFY changed)
		Q_PROPERTY(QByteArray content READ content WRITE setContent NOTIFY changed)

	private:
		int _statusCode;
		QByteArray _content;

	public:
		HttpHandlerFixed(int statusCode = 200, const QByteArray& content = QByteArray(), QObject* parent = 0);

		inline int statusCode() const { return _statusCode; }
		inline const QByteArray& content() const { return _content; }

	public:
		virtual bool handleRequest(Pillow::HttpConnection* connection);

	public slots:
		void setStatusCode(int statusCode);
		void setContent(const QByteArray& content);

	signals:
		void changed();
	};


	//
	// HttpHandler404: a handler that always respond "404 Not Found". Great as the last handler in your chain!
	//

	class PILLOWCORE_EXPORT HttpHandler404 : public HttpHandler
	{
		Q_OBJECT

	public:
		HttpHandler404(QObject* parent = 0);

	public:
		virtual bool handleRequest(Pillow::HttpConnection* connection);
	};


	//
	// HttpHandlerFunction: a handler that invokes a functor, function or lambda.
	//

	class PILLOWCORE_EXPORT HttpHandlerFunction : public HttpHandler
	{
		Q_OBJECT
#ifdef Q_COMPILER_LAMBDA
		std::function<void(Pillow::HttpConnection*)> _function;

	public:
		HttpHandlerFunction(QObject* parent = 0);
		HttpHandlerFunction(const std::function<void(Pillow::HttpConnection*)>& function, QObject* parent = 0);

		inline const std::function<void(Pillow::HttpConnection*)>& function() const { return _function; }
		void setFunction(const std::function<void(Pillow::HttpConnection*)>& function) { _function = function; }

	public:
		virtual bool handleRequest(Pillow::HttpConnection* connection);
#endif // Q_COMPILER_LAMBDA
	};

	//
	// HttpHandlerLog: a handler that logs requests.
	//

	class PILLOWCORE_EXPORT HttpHandlerLog : public HttpHandler
	{
		Q_OBJECT
		Q_ENUMS(Mode)

	public:
		enum Mode
		{
			LogCompletedRequests, // Default: Log requests when they are completed.
			TraceRequests         // Log requests when they are started until when they complete.
		};

	public:
		HttpHandlerLog(QObject* parent = 0);
		HttpHandlerLog(Mode mode, QIODevice* device, QObject* parent = 0);
		HttpHandlerLog(QIODevice* device, QObject* parent = 0);
		~HttpHandlerLog();

		inline Mode mode() const { return _mode; }
		QIODevice* device() const;

	public slots:
		void setMode(Mode mode);
		void setDevice(QIODevice* device);

	public:
		virtual bool handleRequest(Pillow::HttpConnection* connection);

	private slots:
		void requestCompleted(Pillow::HttpConnection* connection);
		void requestClosed(Pillow::HttpConnection* connection);
		void requestDestroyed(QObject* connection);
		void log(const QString& entry);

	private:
		QHash<Pillow::HttpConnection*, QElapsedTimer*> _requestTimerMap;
		Mode _mode;
		QPointer<QIODevice> _device;
	};

	//
	// HttpHandlerFile: a handler that serves static files from the filesystem.
	//

	class PILLOWCORE_EXPORT HttpHandlerFile : public HttpHandler
	{
		Q_OBJECT
		Q_PROPERTY(QString publicPath READ publicPath WRITE setPublicPath NOTIFY changed)

		QString _publicPath;
		int _bufferSize;

	public:
		HttpHandlerFile(const QString& publicPath = QString(), QObject* parent = 0);

		const QString& publicPath() const { return _publicPath; }
		int bufferSize() const { return _bufferSize; }

		enum { DefaultBufferSize = 512 * 1024 };

	public:
		void setPublicPath(const QString& publicPath);
		void setBufferSize(int bytes);

		virtual bool handleRequest(Pillow::HttpConnection* connection);

	signals:
		void changed();
	};

	class PILLOWCORE_EXPORT HttpHandlerFileTransfer : public QObject
	{
		Q_OBJECT
		QPointer<QIODevice> _sourceDevice;
		QPointer<HttpConnection> _connection;
		int _bufferSize;

	public:
		HttpHandlerFileTransfer(QIODevice* sourceDevice, Pillow::HttpConnection* connection, int bufferSize = HttpHandlerFile::DefaultBufferSize);

	public slots:
		void writeNextPayload();

	signals:
		void finished();
	};
}
#endif // PILLOW_HTTPHANDLER_H
