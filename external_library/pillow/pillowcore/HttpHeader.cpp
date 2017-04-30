#include "HttpHeader.h"
#include "ByteArrayHelpers.h"

namespace
{
	inline const QByteArray & nullByteArray()
	{
		static QByteArray null;
		return null;
	}
}

void Pillow::HttpHeader::setFromRawHeader(const char *rawHeader, int len)
{
	if (!rawHeader || len <= 0)
	{
		first = second = QByteArray();
		return;
	}

	if (rawHeader)
	{
		if (len == -1)
			len = qstrlen(rawHeader);
		int colonPos;

		for (colonPos = 0; colonPos < len; ++colonPos)
		{
			if (rawHeader[colonPos] == ':')
				break;
		}

		first = QByteArray(rawHeader, colonPos);

		// Strip colon and spaces after colon.
		if (colonPos < len)
		{
			while (++colonPos < len)
			{
				if (rawHeader[colonPos] != ' ')
					break;
			}

			second = QByteArray(rawHeader + colonPos, len - colonPos);
		}
	}
}

//
// Pillow::HttpHeaderCollection
//

Pillow::HttpHeaderCollection &Pillow::HttpHeaderCollection::operator+=(const QVector<HttpHeader> &l)
{
	int oldSize = size();
	resize(size() + l.size());

	Pillow::HttpHeader *dest = begin() + oldSize;
	const Pillow::HttpHeader *src = l.begin();
	const Pillow::HttpHeader *srcE = l.end();
	while (src != srcE)
		*(dest++) = *(src++);

	return *this;
}

const QByteArray& Pillow::HttpHeaderCollection::getFieldValue(const char *fieldName, int fieldNameLength) const
{
	for (const_iterator it = constBegin(), itE = constEnd(); it != itE; ++it)
	{
		if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(it->first.constData(), it->first.size(), fieldName, fieldNameLength))
			return it->second;
	}
	return nullByteArray();
}

const QByteArray& Pillow::HttpHeaderCollection::getFieldValue(const Pillow::LowerCaseToken &fieldName) const
{
	for (const_iterator it = constBegin(), itE = constEnd(); it != itE; ++it)
	{
		if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(it->first, fieldName))
			return it->second;
	}
	return nullByteArray();
}

QVector<QByteArray> Pillow::HttpHeaderCollection::getFieldValues(const char *fieldName, int fieldNameLength) const
{
	QVector<QByteArray> result;
	for (const_iterator it = constBegin(); it != constEnd(); ++it)
	{
		if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(it->first.constData(), it->first.size(), fieldName, fieldNameLength))
			result << it->second;
	}
	return result;
}

bool Pillow::HttpHeaderCollection::testFieldValue(const char *fieldName, int fieldNameLength, const char *value, int valueLength) const
{
	for (const_iterator it = constBegin(), itE = constEnd(); it != itE; ++it)
	{
		if (it->first.size() != fieldNameLength || it->second.size() != valueLength) continue;

		if (Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(it->first.constData(), it->first.size(), fieldName, fieldNameLength)
			&& Pillow::ByteArrayHelpers::asciiEqualsCaseInsensitive(it->second.constData(), it->second.size(), value, valueLength))
			return true;
	}

	return false;
}
