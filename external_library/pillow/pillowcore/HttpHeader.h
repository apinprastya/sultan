#ifndef PILLOW_HTTPHEADER_H
#define PILLOW_HTTPHEADER_H

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef QPAIR_H
#include <QtCore/QPair>
#endif // QPAIR_H
#ifndef QVECTOR_H
#include <QtCore/QVector>
#endif // QVECTOR_H
#ifndef QBYTEARRAY_H
#include <QtCore/QByteArray>
#endif // QBYTEARRAY_H
#ifndef QMETATYPE_H
#include <QtCore/QMetaType>
#endif // QMETATYPE_H

namespace Pillow { class Token; class LowerCaseToken; }

namespace Pillow
{
	//
	// Pillow::HttpHeader
	//
	// Represents a raw Http header in its original encoding.
	// Essentially a QPair<QByteArray, QByteArray>.
	//
	// Note: This class should have the exact same layout as QPair<QByteArray, QByteArray>,
	// as it allows casting to it.
	//
	struct PILLOWCORE_EXPORT HttpHeader
	{
		inline HttpHeader() : first(), second() {}
		inline HttpHeader(const HttpHeader &other) : first(other.first), second(other.second) {}
		inline HttpHeader(const QPair<QByteArray, QByteArray> &other) : first(other.first), second(other.second) {}
		inline HttpHeader(const QByteArray &first, const QByteArray &second) : first(first), second(second) {}

		// Constructors from string literals, such as: HttpHeader("Field", "Value") and HttpHeader("Field: Value").
		template <int N, int M> inline HttpHeader(const char (&first)[N], const char (&second)[M]) : first(QByteArray(first, N - 1)), second(QByteArray(second, M - 1)) {}
		template <int N> inline HttpHeader(const char (&rawHeader)[N]) { setFromRawHeader(rawHeader, N - 1); }

		inline HttpHeader &operator=(const QPair<QByteArray, QByteArray> &other) { first = other.first; second = other.second; return *this; }
		inline HttpHeader &operator=(const HttpHeader &other) { first = other.first; second = other.second; return *this; }

		//inline operator QPair<QByteArray, QByteArray>() const { return QPair<QByteArray, QByteArray>(first, second); }
		inline operator const QPair<QByteArray, QByteArray>&() const { return *reinterpret_cast<const QPair<QByteArray, QByteArray>*>(this); }

		QByteArray first;
		QByteArray second;

	private:
		void setFromRawHeader(const char* rawHeader, int len = -1);
	};

	//
	// Pillow::HttpHeaderCollection
	//
	// A QVector<HttpHeader> with optimized utility methods to easily find
	// and test for specific headers.
	//
	// Note: This class should have the same layout as QVector<HttpHeader> and safely allow
	// slicing to it.
	//
	class PILLOWCORE_EXPORT HttpHeaderCollection : public QVector<HttpHeader>
	{
	public:
		// Get the value of the first header with the specified field name.
		// The field name is case insensitive.
		const QByteArray& getFieldValue(const char *fieldName, int fieldNameLength) const;
		inline const QByteArray& getFieldValue(const QByteArray &fieldName) const { return getFieldValue(fieldName.constData(), fieldName.size()); }
		template <int N> inline const QByteArray& getFieldValue(const char (&fieldName)[N]) const { return getFieldValue(fieldName, N-1); }
		template <typename T> inline const QByteArray& getFieldValue(const T &fieldName) const { return getFieldValue(fieldName.data(), fieldName.size()); }
		const QByteArray& getFieldValue(const Pillow::LowerCaseToken &fieldName) const;

		// Get values of all headers with the specified field name.
		// The field name is case insensitive.
		QVector<QByteArray> getFieldValues(const char *fieldName, int fieldNameLength) const;
		inline QVector<QByteArray> getFieldValues(const QByteArray &fieldName) const { return getFieldValues(fieldName.constData(), fieldName.size()); }
		template <int N> inline QVector<QByteArray> getFieldValues(const char (&fieldName)[N]) const { return getFieldValues(fieldName, N-1); }

		// Test whether the specified header has the specified value. Both the field and value are case insensitive.
		// If there are multiple headers with the same field, returns whether any of the headers match.
		bool testFieldValue(const char* fieldName, int fieldNameLength, const char* value, int valueLength) const;
		template <int N, int M> inline bool testFieldValue(const char (&fieldName)[N], const char (&value)[M]) const { return testFieldValue(fieldName, N - 1, value, M - 1); }
		template <typename T, typename U> inline bool testFieldValue(const T &fieldName, const U &value) const { return testFieldValue(fieldName.data(), fieldName.size(), value.data(), value.size()); }
		template <int N, typename U> inline bool testFieldValue(const char (&fieldName)[N], const U &value) const { return testFieldValue(fieldName, N - 1, value.data(), value.size()); }
		template <typename T, int M> inline bool testFieldValue(const T &fieldName, const char (&value)[M]) const { return testFieldValue(fieldName.data(), fieldName.size(), value, M - 1); }

	public:
		// Base methods reimplemented to return HttpHeaderCollection instead of QVector<HttpHeader>.
		HttpHeaderCollection &operator+=(const QVector<HttpHeader> &l);
		inline HttpHeaderCollection operator+(const QVector<HttpHeader> &l) const { HttpHeaderCollection n = *this; n += l; return n; }
		inline HttpHeaderCollection &operator+=(const HttpHeader &t) { append(t); return *this; }
		inline HttpHeaderCollection &operator<<(const HttpHeader &t) { append(t); return *this; }
		inline HttpHeaderCollection &operator<<(const QVector<HttpHeader> &l) { *this += l; return *this; }
		inline operator const QVector<QPair<QByteArray, QByteArray> > &() const { return *reinterpret_cast<const QVector<QPair<QByteArray, QByteArray> >*>(this); }
	};

	inline bool operator==(const Pillow::HttpHeader &p1, const QPair<QByteArray, QByteArray> &p2) { return p1.first == p2.first && p1.second == p2.second; }
	inline bool operator==(const Pillow::HttpHeader &p1, const Pillow::HttpHeader &p2) { return p1.first == p2.first && p1.second == p2.second; }

	inline bool operator!=(const Pillow::HttpHeader &p1, const QPair<QByteArray, QByteArray> &p2) { return !(p1 == p2); }
	inline bool operator!=(const Pillow::HttpHeader &p1, const Pillow::HttpHeader &p2) { return !(p1 == p2); }
}

Q_DECLARE_METATYPE(Pillow::HttpHeader);
Q_DECLARE_METATYPE(Pillow::HttpHeaderCollection);

#endif // PILLOW_HTTPHEADER_H
