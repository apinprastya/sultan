#include "HttpHandlerQtScript.h"
#include "HttpConnection.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QtCore/QUrl>
#include <QFile>
#include <QFileInfo>
using namespace Pillow;

static QScriptValue toScriptValue(QScriptEngine *engine, const Pillow::HttpHeaderCollection &headers)
{
	QScriptValue result = engine->newObject();

	for (int i = 0, iE = headers.size(); i < iE; ++i)
		result.setProperty(QString::fromUtf8(headers.at(i).first), QUrl::fromPercentEncoding(headers.at(i).second));

	return result;
}

static void fromScriptValue(const QScriptValue &obj, Pillow::HttpHeaderCollection &headers)
{
	for (QScriptValueIterator it(obj); it.hasNext();)
	{
		it.next();
		headers.append(HttpHeader(it.name().toUtf8(), it.value().toString().toUtf8()));
	}
}

static void registerMarshallers(QScriptEngine* engine)
{
	if (engine == NULL) return;
	if (engine->property("_HttpHandlerQtScript_registered").toBool() == true) return;
	engine->setProperty("_HttpHandlerQtScript_registered", true);

	qScriptRegisterMetaType<Pillow::HttpHeaderCollection>(engine, toScriptValue, fromScriptValue);
}

static QString objectToString(const QScriptValue &object)
{
	QString result;
	result.append(object.toString()).append("\n");
	for (QScriptValueIterator it(object); it.hasNext();)
	{
		it.next();
		result.append(QString("%1: %2\n").arg(it.name(), it.value().toString()));
	}
	return result;
}

//
// HttpHandlerQtScript
//

HttpHandlerQtScript::HttpHandlerQtScript(QScriptValue scriptFunction, QObject *parent)
: HttpHandler(parent)
{
	setScriptFunction(scriptFunction);
}

HttpHandlerQtScript::HttpHandlerQtScript(QObject *parent)
: HttpHandler(parent)
{
}

void HttpHandlerQtScript::setScriptFunction(const QScriptValue &scriptFunction)
{
	if (_scriptFunction.strictlyEquals(scriptFunction)) return;
	_scriptFunction = scriptFunction;

	if (!_scriptFunction.isValid())
		qWarning() << "HttpHandlerQtScript::setScriptFunction: passed an invalid QScriptValue; this handler will not be able to handle requests.";
	else if (!_scriptFunction.isFunction())
		qWarning() << "HttpHandlerQtScript::setScriptFunction: passed a non-function QScriptValue; this handler will not be able to handle requests.";
	else
		registerMarshallers(scriptFunction.engine());
}

bool HttpHandlerQtScript::handleRequest(Pillow::HttpConnection *connection)
{
	if (!_scriptFunction.isFunction()) return false;

	QScriptEngine* engine = _scriptFunction.engine();
	QScriptValue requestObject = engine->newObject();
	requestObject.setProperty("nativeRequest", _scriptFunction.engine()->newQObject(connection));
	requestObject.setProperty("requestMethod", QUrl::fromPercentEncoding(connection->requestMethod()));
	requestObject.setProperty("requestUri", QUrl::fromPercentEncoding(connection->requestUri()));
	requestObject.setProperty("requestFragment", QUrl::fromPercentEncoding(connection->requestFragment()));
	requestObject.setProperty("requestPath", QUrl::fromPercentEncoding(connection->requestPath()));
	requestObject.setProperty("requestQueryString", QUrl::fromPercentEncoding(connection->requestQueryString()));
	requestObject.setProperty("requestHeaders", qScriptValueFromValue(engine, connection->requestHeaders()));
	requestObject.setProperty("requestContent", QUrl::fromPercentEncoding(connection->requestContent()));

	QList<QPair<QString, QString> > queryParams = QUrl(connection->requestUri()).queryItems();
	QScriptValue queryParamsObject = engine->newObject();
	for (int i = 0, iE = queryParams.size(); i < iE; ++i)
		queryParamsObject.setProperty(queryParams.at(i).first, queryParams.at(i).second);
	requestObject.setProperty("requestQueryParams", queryParamsObject);

	QScriptValue result = _scriptFunction.call(_scriptFunction, QScriptValueList() << requestObject);

	if (result.isError())
	{
		if (connection->state() == HttpConnection::SendingHeaders)
		{
			// Nothing was sent yet... We have a chance to let the client know we had an error.
			connection->writeResponseString(500, HttpHeaderCollection(), objectToString(result));
		}
		engine->clearExceptions();
		return true;
	}

	return result.toBool();
}

//
// HttpHandlerQtScriptFile
//

HttpHandlerQtScriptFile::HttpHandlerQtScriptFile(QScriptEngine* engine, const QString &fileName, const QString &functionName, bool autoReload, QObject *parent)
: HttpHandlerQtScript(parent)
{
	_scriptObject = engine->newObject();
	setFileName(fileName);
	setFunctionName(functionName);
	setAutoReload(autoReload);
}

void HttpHandlerQtScriptFile::setFileName(const QString &fileName)
{
	if (_fileName == fileName) return;
	_fileName = fileName;

	if (!QFile::exists(fileName))
		qWarning() << "HttpHandlerQtScriptFile::setFileName: file" << fileName << "does not exist. This handler won't be able to handle requests.";
}

void HttpHandlerQtScriptFile::setFunctionName(const QString &functionName)
{
	if (_functionName == functionName) return;
	_functionName = functionName;
}

void HttpHandlerQtScriptFile::setAutoReload(bool autoReload)
{
	if (_autoReload == autoReload) return;
	_autoReload = autoReload;
}

bool HttpHandlerQtScriptFile::handleRequest(Pillow::HttpConnection *request)
{
	if (!_lastModified.isValid() || (_autoReload && QFileInfo(_fileName).lastModified() > _lastModified))
	{
		// Time to (re)load the script.
		qDebug() << "HttpHandlerQtScriptFile::handleRequest: (re)loading" << _fileName;
		QFile file(_fileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			request->writeResponseString(500, HttpHeaderCollection(), QString("HttpHandlerQtScriptFile::handleRequest: Could not read file %1").arg(_fileName));
			return true;
		}

		QScriptEngine* engine = _scriptObject.engine();
		_scriptObject = engine->newObject();
		QScriptContext* context = engine->pushContext();
		context->setActivationObject(_scriptObject);
		context->setThisObject(_scriptObject);
		QScriptValue result = engine->evaluate(file.readAll(), _fileName);
		engine->popContext();

		if (result.isError())
		{
			request->writeResponseString(500, HttpHeaderCollection(), QString("HttpHandlerQtScriptFile::handleRequest: Error while evaluating script %1: %2").arg(_fileName).arg(objectToString(result)));
			return true;
		}

		QScriptValue scriptFunction = _scriptObject.property(_functionName);
		if (!scriptFunction.isFunction())
		{
			request->writeResponseString(500, HttpHeaderCollection(), QString("HttpHandlerQtScriptFile::handleRequest: Error while evaluating script %1: '%2' is not a function defined in the script").arg(_fileName).arg(_functionName));
			return true;
		}

		_lastModified = QFileInfo(_fileName).lastModified();
		setScriptFunction(scriptFunction);
	}

	return HttpHandlerQtScript::handleRequest(request);
}
