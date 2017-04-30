#include "HttpHelpers.h"
#include "ByteArrayHelpers.h"

namespace Pillow
{
	namespace HttpMimeHelper
	{
		const char* getMimeTypeForFilename(const QString& filename)
		{
			if (filename.endsWith(".html", Qt::CaseInsensitive)) return "text/html";
			if (filename.endsWith(".jpg", Qt::CaseInsensitive)) return "image/jpeg";
			if (filename.endsWith(".jpeg", Qt::CaseInsensitive)) return "image/jpeg";
			if (filename.endsWith(".png", Qt::CaseInsensitive)) return "image/png";
			if (filename.endsWith(".gif", Qt::CaseInsensitive)) return "image/gif";
			if (filename.endsWith(".css", Qt::CaseInsensitive)) return "text/css";
			if (filename.endsWith(".js", Qt::CaseInsensitive)) return "text/javascript";
			if (filename.endsWith(".xml", Qt::CaseInsensitive)) return "text/xml";
			return "application/octet-stream";
		}
	}

	namespace HttpProtocol
	{
		namespace StatusCodes
		{
			const char* getStatusCodeAndMessage(int statusCode)
			{
				switch (statusCode)
				{
					case 100: return "100 Continue";
					case 101: return "101 Switching Protocols";

					case 200: return "200 OK";
					case 201: return "201 Created";
					case 202: return "202 Accepted";
					case 203: return "203 Non-Authoritative Information";
					case 204: return "204 No Content";
					case 205: return "205 Reset Content";
					case 206: return "206 Partial Content";

					case 300: return "300 Multiple Choices";
					case 301: return "301 Moved Permanently";
					case 302: return "302 Found";
					case 303: return "303 See Other";
					case 304: return "304 Not Modified";
					case 305: return "305 Use Proxy";
					case 307: return "307 Temporary Redirect";

					case 400: return "400 Bad Request";
					case 401: return "401 Unauthorized";
					case 403: return "403 Forbidden";
					case 404: return "404 Not Found";
					case 405: return "405 Method Not Allowed";
					case 406: return "406 Not Acceptable";
					case 407: return "407 Proxy Authentication Required";
					case 408: return "408 Request Timeout";
					case 409: return "409 Conflict";
					case 410: return "410 Gone";
					case 411: return "411 Length Required";
					case 412: return "412 Precondition Failed";
					case 413: return "413 Request Entity Too Large";
					case 414: return "414 Request-URI Too Long";
					case 415: return "415 Unsupported Media Type";
					case 416: return "416 Requested Range Not Satisfiable";
					case 417: return "417 Expectation Failed";

					case 500: return "500 Internal Server Error";
					case 501: return "501 Not Implemented";
					case 502: return "502 Bad Gateway";
					case 503: return "503 Service Unavailable";
					case 504: return "504 Gateway Timeout";
					case 505: return "505 HTTP Version Not Supported";
				}
				return NULL;
			}

			const char* getStatusMessage(int statusCode)
			{
				const char* statusCodeAndMessage = getStatusCodeAndMessage(statusCode);
				return statusCodeAndMessage == NULL ? NULL : statusCodeAndMessage + 4;
			}
		}

		namespace Dates
		{
			QByteArray getHttpDate(const QDateTime& dateTime /*= QDateTime::currentDateTime()*/)
			{
				static const char* dayNames[] = {NULL, "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
				static const char* monthNames[] = {NULL, "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
				static const char* intNames[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60"};

				QDateTime utcDateTime = dateTime.toUTC();
				const QDate& date = utcDateTime.date();
				const QTime& time = utcDateTime.time();

				QByteArray httpDate; httpDate.reserve(32);
				httpDate.append(dayNames[date.dayOfWeek()]).append(", ").append(intNames[date.day()]).append(' ').append(monthNames[date.month()]).append(' ');
				Pillow::ByteArrayHelpers::appendNumber<int, 10>(httpDate, date.year());
				httpDate.append(' ').append(intNames[time.hour()]).append(':').append(intNames[time.minute()]).append(':').append(intNames[time.second()]).append(" GMT");

				return httpDate;
			}
		}
	}
}
