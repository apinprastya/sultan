#include "HttpHandlerSimpleRouter.h"
#include "HttpConnection.h"
#include <QtCore/QPointer>
#include <QtCore/QMetaMethod>
#include <QtCore/QRegExp>
#include <QtCore/QVarLengthArray>
#include <QtCore/QUrl>
using namespace Pillow;

static const QString methodToken("_method");

namespace Pillow
{
	struct Route
	{
		QByteArray method;
		QRegExp regExp;
		QStringList paramNames;

		virtual ~Route() {}
		virtual bool invoke(Pillow::HttpConnection* request) = 0;
	};

	struct HandlerRoute : public Route
	{
		QPointer<Pillow::HttpHandler> handler;

		virtual bool invoke(Pillow::HttpConnection *request)
		{
			if (!handler) return false;
			return handler->handleRequest(request);
		}
	};

	struct QObjectMetaCallRoute : public Route
	{
		QPointer<QObject> object;
		QByteArray member;

		virtual bool invoke(Pillow::HttpConnection *request)
		{
			if (!object) return false;
			return QMetaObject::invokeMethod(object, member.constData(), Q_ARG(Pillow::HttpConnection*, request));
		}
	};

	struct QObjectMethodCallRoute : public Route
	{
		QPointer<QObject> object;
		QMetaMethod metaMethod;

		virtual bool invoke(Pillow::HttpConnection *request)
		{
			if (!object) return false;
			return metaMethod.invoke(object, Q_ARG(Pillow::HttpConnection*, request));
		}
	};

#ifdef Q_COMPILER_LAMBDA
	struct FunctorCallRoute : public Route
	{
		std::function<void(Pillow::HttpConnection*)> func;

		virtual bool invoke(Pillow::HttpConnection *request)
		{
			func(request);
			return true;
		}
	};
#endif // Q_COMPILER_LAMBDA

	struct StaticRoute : public Route
	{
		int statusCode;
		Pillow::HttpHeaderCollection headers;
		QByteArray content;

		virtual bool invoke(Pillow::HttpConnection *request)
		{
			request->writeResponse(statusCode, headers, content);
			return true;
		}
	};

	//
	// HttpHandlerSimpleRouterPrivate
	//

	class HttpHandlerSimpleRouterPrivate
	{
	public:
		QList<Route*> routes;
		HttpHandlerSimpleRouter::RoutingErrorAction methodMismatchAction;
		HttpHandlerSimpleRouter::RoutingErrorAction unmatchedRequestAction;
		bool acceptMethodParam;
	};
}

//
// HttpHandlerSimpleRouter
//

HttpHandlerSimpleRouter::HttpHandlerSimpleRouter(QObject* parent /* = 0 */)
	: Pillow::HttpHandler(parent), d_ptr(new HttpHandlerSimpleRouterPrivate)
{
	d_ptr->methodMismatchAction = Passthrough;
	d_ptr->unmatchedRequestAction = Passthrough;
	d_ptr->acceptMethodParam = false;
}

HttpHandlerSimpleRouter::~HttpHandlerSimpleRouter()
{
	foreach (Route* route, d_ptr->routes)
		delete route;
	delete d_ptr;
}

void HttpHandlerSimpleRouter::addRoute(const QByteArray& method, const QString &path, Pillow::HttpHandler *handler)
{
	HandlerRoute* route = new HandlerRoute();
	route->method = method;
	route->regExp = pathToRegExp(path, &route->paramNames);
	route->handler = handler;
	d_ptr->routes.append(route);
}

void HttpHandlerSimpleRouter::addRoute(const QByteArray& method, const QString& path, QObject* object, const char* member)
{
	if (object == NULL)
	{
		qWarning() << "HttpHandlerSimpleRouter::addRoute: NULL target object specified while adding route for" << path << "- not adding route";
		return;
	}
	else if (member == NULL || member[0] == 0)
	{
		qWarning() << "HttpHandlerSimpleRouter::addRoute: NULL or empty member specified while adding route for" << path << "- not adding route";
		return;
	}

	if (member[0] == '1')
	{
		// This is a slot name produced with the SLOT() macro, skip the leading id.
		member++;
	}
	int methodIndex = object->metaObject()->indexOfSlot(member);
	if (methodIndex == -1) object->metaObject()->indexOfMethod(member);

	if (methodIndex == -1)
	{
		// Not a normalised method name. Still give a chance and invoke the member by name.
		QObjectMetaCallRoute* route = new QObjectMetaCallRoute();
		route->method = method;
		route->regExp = pathToRegExp(path, &route->paramNames);
		route->object = object;
		route->member = member;
		d_ptr->routes.append(route);
	}
	else
	{
		QObjectMethodCallRoute* route = new QObjectMethodCallRoute();
		route->method = method;
		route->regExp = pathToRegExp(path, &route->paramNames);
		route->object = object;
		route->metaMethod = object->metaObject()->method(methodIndex);
		d_ptr->routes.append(route);
	}
}

void HttpHandlerSimpleRouter::addRoute(const QByteArray& method, const QString& path, int statusCode, const Pillow::HttpHeaderCollection& headers, const QByteArray& content /*= QByteArray()*/)
{
	StaticRoute* route = new StaticRoute();
	route->method = method;
	route->regExp = pathToRegExp(path, &route->paramNames);
	route->statusCode = statusCode;
	route->headers = headers;
	route->content = content;
	d_ptr->routes.append(route);
}

#ifdef Q_COMPILER_LAMBDA
void HttpHandlerSimpleRouter::addRoute(const QByteArray &method, const QString &path, const std::function<void(HttpConnection *)> &func)
{
	FunctorCallRoute* route = new FunctorCallRoute();
	route->method = method;
	route->regExp = pathToRegExp(path, &route->paramNames);
	route->func = func;
	d_ptr->routes.append(route);
}
#endif // Q_COMPILER_LAMBDA

QRegExp Pillow::HttpHandlerSimpleRouter::pathToRegExp(const QString &p, QStringList* outParamNames)
{
	QString path = p;

	QRegExp paramRegex(":(\\w+)"); QString paramReplacement("([\\w_-]+)");
	QStringList paramNames;
	int pos = 0;
	while (pos >= 0)
	{
		pos = paramRegex.indexIn(path, pos);
		if (pos >= 0)
		{
			paramNames << paramRegex.cap(1);
			pos += paramRegex.matchedLength();
		}
	}
	path.replace(paramRegex, paramReplacement);

	QRegExp splatRegex("\\*(\\w+)"); QString splatReplacement("(.*)");
	pos = 0;
	while (pos >= 0)
	{
		pos = splatRegex.indexIn(path, pos);
		if (pos >= 0)
		{
			paramNames << splatRegex.cap(1);
			pos += splatRegex.matchedLength();
		}
	}
	path.replace(splatRegex, splatReplacement);

	if (outParamNames)
		*outParamNames = paramNames;

	path = "^" + path + "$";
	return QRegExp(path);
}

bool HttpHandlerSimpleRouter::handleRequest(Pillow::HttpConnection *request)
{
	QVarLengthArray<Route*, 16> matchedRoutes;

	QByteArray requestMethod = request->requestMethod();
	if (d_ptr->acceptMethodParam)
	{
		QString methodParam = request->requestParamValue(methodToken);
		if (!methodParam.isEmpty())
			requestMethod = methodParam.toAscii();
	}

	QString requestPath = QUrl::fromPercentEncoding(request->requestPath());

	foreach (Route* route, d_ptr->routes)
	{
		if (route->regExp.indexIn(requestPath) != -1)
		{
			matchedRoutes.append(route);
			if (route->method.isEmpty() ||
				(route->method.size() == requestMethod.size() && qstricmp(route->method, requestMethod) == 0))
			{
				for (int i = 0, iE = route->paramNames.size(); i < iE; ++i)
					request->setRequestParam(route->paramNames.at(i), route->regExp.cap(i + 1));
				route->invoke(request);
				return true;
			}
		}
	}

	if (matchedRoutes.isEmpty())
	{
		if (unmatchedRequestAction() == Return4xxResponse)
		{
			request->writeResponse(404);
			return true;
		}
	}
	else
	{
		if (methodMismatchAction() == Return4xxResponse)
		{
			QByteArray allowedMethods;
			for (int i = 0, iE = matchedRoutes.size(); i < iE; ++i)
			{
				if (i > 0) allowedMethods.append(", ");
				allowedMethods.append(matchedRoutes.at(i)->method);
			}
			request->writeResponse(405, HttpHeaderCollection() << HttpHeader("Allow", allowedMethods));
			return true;
		}
	}

	return false;
}

Pillow::HttpHandlerSimpleRouter::RoutingErrorAction Pillow::HttpHandlerSimpleRouter::unmatchedRequestAction() const
{
	return d_ptr->unmatchedRequestAction;
}

void Pillow::HttpHandlerSimpleRouter::setUnmatchedRequestAction(Pillow::HttpHandlerSimpleRouter::RoutingErrorAction action)
{
	d_ptr->unmatchedRequestAction = action;
}

Pillow::HttpHandlerSimpleRouter::RoutingErrorAction Pillow::HttpHandlerSimpleRouter::methodMismatchAction() const
{
	return d_ptr->methodMismatchAction;
}

void Pillow::HttpHandlerSimpleRouter::setMethodMismatchAction(Pillow::HttpHandlerSimpleRouter::RoutingErrorAction action)
{
	d_ptr->methodMismatchAction = action;
}

bool Pillow::HttpHandlerSimpleRouter::acceptsMethodParam() const
{
	return d_ptr->acceptMethodParam;
}

void Pillow::HttpHandlerSimpleRouter::setAcceptsMethodParam(bool accept)
{
	d_ptr->acceptMethodParam = accept;
}
