#ifndef DB_GLOBAL_H
#define DB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DB_LIBRARY_STATIC
    #define DBSHARED_EXPORT
#else
    #if defined(DB_LIBRARY)
        #define DBSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define DBSHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // DB_GLOBAL_H
