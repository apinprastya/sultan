#ifndef PILLOW_BYTEARRAYHELPERS_H
#define PILLOW_BYTEARRAYHELPERS_H

#ifndef QBYTEARRAY_H
#include <QtCore/QByteArray>
#endif // QBYTEARRAY_H
#ifndef QSTRING_H
#include <QtCore/QString>
#endif // QSTRING_H
#ifndef QSTRINGBUILDER_H
#include <QtCore/QStringBuilder>
#endif // QSTRINGBUILDER_H

namespace Pillow
{
	//
	// Pillow::Token. A char array literal. See QLatin1Literal.
	//
	class Token
	{
	public:
		inline int size() const { return m_size; }
		inline const char* data() const { return m_data; }
		inline char at(int index) const { return m_data[index]; }
		template <int N> inline Token(const char (&str)[N]) : m_size(N - 1), m_data(str) {}
	private:
		const int m_size;
		const char * const m_data;
	};

	//
	// Pillow::Token. A char array literal with the user-given assertion that it only contains lowercase data.
	//
	class LowerCaseToken
	{
	public:
		inline int size() const { return m_size; }
		inline const char* data() const { return m_data; }
		inline char at(int index) const { return m_data[index]; }
		template <int N> inline LowerCaseToken(const char (&str)[N]) : m_size(N - 1), m_data(str) {}
	private:
		const int m_size;
		const char * const m_data;
	};


	//
	// Pillow::ByteArrayHelpers
	//
	// This namespace contains functions that augment or replaces methods already
	// available on QByteArray. However, they are highly performance optimized for Pillow's
	// internal use cases. Please use the QByteArray equivalents unless you identify that
	// these solve a real performance issue in your code.
	//
	namespace ByteArrayHelpers
	{
		inline void setFromRawDataAndNullterm(QByteArray& target, char* data, int start, int length)
		{
			// Switching between a deep copy/unshared target QByteArray and a shared data QByteArray
			// will *not* cause memory leaks as Qt allocates and frees the QByteArray control data+buffer in one block.
			// So the only downside that will happen if an unshared QByteArray is altered to share data is a few bytes wasted
			// until the QByteArray control block releases the control data + the unshared buffer at some point in the future.

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			if (length == 0)
			{
				target.setRawData("", 0);
			}
			else
			{
				target.setRawData(data + start, length);
				*(data + start + length) = 0; // Null terminate the string.
			}

#else
			target.data_ptr()->alloc = 0;
			if (length == 0)
				target.setRawData("", 0);
			else
			{
				if (target.data_ptr()->ref == 1)
				{
					target.data_ptr()->data = data + start;
					target.data_ptr()->alloc = target.data_ptr()->size = length;
					target.data_ptr()->array[0] = 0;
				}
				else
					target.setRawData(data + start, length);
				*(data + start + length) = 0; // Null terminate the string.
			}
#endif
		}

		inline void setFromRawData(QByteArray& target, const char* data, int start, int length)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			target.setRawData(data + start, length);
#else
			if (target.data_ptr()->ref == 1)
			{
				target.data_ptr()->data = const_cast<char*>(data) + start;
				target.data_ptr()->alloc = target.data_ptr()->size = length;
				target.data_ptr()->array[0] = 0;
			}
			else
			{
				target.data_ptr()->alloc = 0;
				target.setRawData(data + start, length);
			}
#endif
		}

		template <typename Integer, int Base>
		inline void appendNumber(QByteArray& target, const Integer number)
		{
			static const char intToChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
			int start = target.size();
			Integer n = number; if (n < 0) n = -n;
			do { if (Base <= 10) target.append(char(n % Base) + '0'); else target.append(intToChar[n % Base]); } while ((n /= Base) > 0);
			if (number < 0) target.append('-');

			// Reverse the string we've just output
			int end = target.size() - 1;
			char* data = target.data();
			while (start < end)
			{
				char c = data[start];
				data[start] = data[end];
				data[end] = c;
				++start; --end;
			}
		}

		inline bool asciiEqualsCaseInsensitive(const QByteArray& first, const QByteArray& second)
		{
			if (first.size() != second.size()) return false;
			for (register int i = 0; i < first.size(); ++i)
			{
				register char f = first.at(i), s = second.at(i);
				bool good = (f == s) || ((f - s) == 32 && f >= 'a' && f <= 'z') || ((f - s) == -32 && f >= 'A' && f <= 'Z');
				if (!good) return false;
			}
			return true;
		}

		inline bool asciiEqualsCaseInsensitive(const QByteArray& first, const QLatin1Literal& second)
		{
			if (first.size() != second.size()) return false;
			const char* sec = second.data();
			for (register int i = 0; i < first.size(); ++i)
			{
				register char f = first.at(i), s = sec[i];
				bool good = (f == s) || ((f - s) == 32 && f >= 'a' && f <= 'z') || ((f - s) == -32 && f >= 'A' && f <= 'Z');
				if (!good) return false;
			}
			return true;
		}

		inline bool asciiEqualsCaseInsensitive(const char* first, int firstSize, const char* second, int secondSize)
		{
			if (firstSize != secondSize) return false;
			for (register int i = 0; i < firstSize; ++i)
			{
				register char f = first[i], s = second[i];
				bool good = (f == s) || ((f - s) == 32 && f >= 'a' && f <= 'z') || ((f - s) == -32 && f >= 'A' && f <= 'Z');
				if (!good) return false;
			}
			return true;
		}

		inline bool asciiEqualsCaseInsensitive(const QByteArray& first, const Pillow::Token& second)
		{
			if (first.size() != second.size()) return false;
			for (register int i = 0; i < first.size(); ++i)
			{
				register char f = first.at(i), s = second.at(i);
				bool good = (f == s) || ((f - s) == 32 && f >= 'a' && f <= 'z') || ((f - s) == -32 && f >= 'A' && f <= 'Z');
				if (!good) return false;
			}
			return true;
		}

		inline bool asciiEqualsCaseInsensitive(const QByteArray& first, const Pillow::LowerCaseToken& second)
		{
			if (first.size() != second.size()) return false;
			for (register int i = 0; i < first.size(); ++i)
			{
				register char f = first.at(i), s = second.at(i);
				bool good = (f == s) || ((f - s) == -32 && f >= 'A' && f <= 'Z');
				if (!good) return false;
			}
			return true;
		}

		inline char unhex(const char c)
		{
			if (c >= '0' && c <= '9') return c - '0';
			if (c >= 'a' && c <= 'f') return (c - 'a') + 10;
			if (c >= 'A' && c <= 'F') return (c - 'A') + 10;
			return 0;
		}

		inline int percentDecodeInPlace(char* data, int size)
		{
			const char* inData = data;
			const char* inDataE = data + size;
			while (inData < inDataE)
			{
				if (inData[0] == '%' && inData + 2 < inDataE)
				{
					// Percent-encoded sequence.
					*data = static_cast<char>(unhex(inData[1]) << 4 | unhex(inData[2]));
					++data; inData += 3;
					size -= 2;
				}
				else
				{
					*data = *inData;
					++data; ++inData;
				}
			}
			return size;
		}

		inline QString percentDecode(const QByteArray& byteArray)
		{
			if (byteArray.isEmpty()) return QString();

			if (byteArray.size() < 1024)
			{
				char buffer[1024];
				memcpy(buffer, byteArray.constData(), byteArray.size());
				return QString::fromUtf8(buffer, percentDecodeInPlace(buffer, byteArray.size()));
			}
			else
			{
				QByteArray temp = byteArray; temp.detach();
				return QString::fromUtf8(temp.constData(), percentDecodeInPlace(temp.data(), byteArray.size()));
			}
		}

		inline QString percentDecode(const char* data, int size)
		{
			if (size == 0 || data == 0) return QString();
			if (size < 1024)
			{
				char buffer[1024];
				memcpy(buffer, data, size);
				return QString::fromUtf8(buffer, percentDecodeInPlace(buffer, size));
			}
			else
			{
				QByteArray temp(data, size);
				return QString::fromUtf8(temp.constData(), percentDecodeInPlace(temp.data(), size));
			}
		}

	}
}

#endif // PILLOW_BYTEARRAYHELPERS_H
