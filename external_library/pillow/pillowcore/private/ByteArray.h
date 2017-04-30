#ifndef PILLOW_BYTEARRAY_H
#define PILLOW_BYTEARRAY_H

#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII

#ifndef PILLOW_BYTEARRAYHELPERS_H
#include "../ByteArrayHelpers.h"
#endif // PILLOW_BYTEARRAYHELPERS_H

namespace Pillow
{
	//
	// Pillow::ByteArray
	//
	// A QByteArray optimized for Pillow::HttpConnection's internal use case of
	// building a response through repeated calls to the various append() overloads.
	//
	// It does the following assumptions:
	// - It is not shared.
	// - It has enough space reserved to append data, else will take a greater performance hit.
	// - append(const char*) will never be called. Instead append(const char*, int) or append(const char&[]) will.
	//   calling append(const char*) will actually use the append(const QByteArray& ) overload, causing a massive
	//   performance hit. The append(const char*) overload is disabled because it prevent using the append(const char&[]) overload.
	//
	class ByteArray : public QByteArray
	{
	public:
		inline ByteArray& operator =(const QByteArray& other)
		{
			QByteArray::operator =(other);
			return *this;
		}

		inline bool operator==(const QLatin1Literal& literal) const
		{
			return size() == literal.size() && qstrncmp(constData(), literal.data(), size()) == 0;
		}

		inline bool operator!=(const QLatin1Literal& literal) const
		{
			return size() != literal.size() || qstrncmp(constData(), literal.data(), size()) != 0;
		}

		inline bool operator==(const Pillow::Token& literal) const
		{
			return size() == literal.size() && qstrncmp(constData(), literal.data(), size()) == 0;
		}

		inline bool operator!=(const Pillow::Token& literal) const
		{
			return size() != literal.size() || qstrncmp(constData(), literal.data(), size()) != 0;
		}

		inline bool operator==(const Pillow::LowerCaseToken& literal) const
		{
			return size() == literal.size() && qstrncmp(constData(), literal.data(), size()) == 0;
		}

		inline bool operator!=(const Pillow::LowerCaseToken& literal) const
		{
			return size() != literal.size() || qstrncmp(constData(), literal.data(), size()) != 0;
		}

		inline ByteArray& append(const Pillow::HttpHeader& header)
		{
			const int requiredSpace = header.first.size() + 2 + header.second.size() + 2;
			if (capacity() >= size() + requiredSpace)
			{
				// Common case where there is enough space.
				char* d = data() + size();
				memcpy(d, header.first.constData(), header.first.size()); d += header.first.size();
				d[0] = ':'; d[1] = ' '; d += 2;
				memcpy(d, header.second.constData(), header.second.size()); d += header.second.size();
				d[0] = '\r'; d[1] = '\n';
				data_ptr()->size += requiredSpace;
			}
			else
				QByteArray::append(header.first).append(": ", 2).append(header.second).append("\r\n", 2);
			return *this;
		}

		inline ByteArray& append(const QLatin1Literal& literal)
		{
			if (capacity() >= size() + literal.size())
			{
				memcpy(data() + size(), literal.data(), literal.size());
				data_ptr()->size += literal.size();
			}
			else
				QByteArray::append(literal.data(), literal.size());
			return *this;
		}

		inline ByteArray& append(const Pillow::Token& literal)
		{
			if (capacity() >= size() + literal.size())
			{
				memcpy(data() + size(), literal.data(), literal.size());
				data_ptr()->size += literal.size();
			}
			else
				QByteArray::append(literal.data(), literal.size());
			return *this;
		}

		inline ByteArray& append(const Pillow::LowerCaseToken& literal)
		{
			if (capacity() >= size() + literal.size())
			{
				memcpy(data() + size(), literal.data(), literal.size());
				data_ptr()->size += literal.size();
			}
			else
				QByteArray::append(literal.data(), literal.size());
			return *this;
		}

		inline ByteArray& append(const QByteArray& a)
		{
			if (capacity() >= size() + a.size())
			{
				memcpy(data() + size(), a.constData(), a.size());
				data_ptr()->size += a.size();
			}
			else
				QByteArray::append(a);
			return *this;
		}

		template <int N> inline ByteArray& append(const char (&str)[N])
		{
			if (capacity() >= size() + (N - 1))
			{
				memcpy(data() + size(), str, N - 1);
				data_ptr()->size += (N - 1);
			}
			else
				QByteArray::append(str, (N - 1));
			return *this;
		}

		inline ByteArray& append(const char* s, int len)
		{
			if (capacity() >= size() + len)
			{
				memcpy(data() + size(), s, len);
				data_ptr()->size += len;
			}
			else
				QByteArray::append(s, len);
			return *this;
		}

		inline ByteArray& append(char c)
		{
			if (capacity() >= size() + 1)
			{
				data()[size()] = c;
				data_ptr()->size += 1;
			}
			else
				QByteArray::append(c);
			return *this;
		}
	};
}
#endif // PILLOW_BYTEARRAY_H
