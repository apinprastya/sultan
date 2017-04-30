#ifndef GLOBAL_GLOBAL_H
#define GLOBAL_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GLOBAL_LIBRARY_STATIC
    #define GLOBALSHARED_EXPORT
#else
    #if defined(GLOBAL_LIBRARY)
        #define GLOBALSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define GLOBALSHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // GLOBAL_GLOBAL_H
