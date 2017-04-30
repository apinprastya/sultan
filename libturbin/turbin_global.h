#ifndef TURBIN_GLOBAL_H
#define TURBIN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef TURBIN_LIBRARY_STATIC
    #define TURBINSHARED_EXPORT
#else
    #if defined(TURBIN_LIBRARY)
        #define TURBINSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define TURBINSHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // TURBIN_GLOBAL_H
