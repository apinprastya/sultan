#ifndef PILLOW_HTTPHELPERS_H
#define PILLOW_HTTPHELPERS_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QSTRING_H
#include <QString>
#endif // QSTRING_H
#ifndef QBYTEARRAY_H
#include <QByteArray>
#endif // QBYTEARRAY_H
#ifndef QDATETIME_H
#include <QDateTime>
#endif // QDATETIME_H

namespace Pillow
{
	namespace HttpMimeHelper
	{
		PILLOWCORE_EXPORT const char* getMimeTypeForFilename(const QString& filename);
	}

	namespace HttpProtocol
	{
		namespace StatusCodes
		{
			PILLOWCORE_EXPORT const char* getStatusCodeAndMessage(int statusCode);
			PILLOWCORE_EXPORT const char* getStatusMessage(int statusCode);
		}

		namespace Dates
		{
			PILLOWCORE_EXPORT QByteArray getHttpDate(const QDateTime& dateTime = QDateTime::currentDateTime());
		}
	}
}

#endif // PILLOW_HTTPHELPERS_H
