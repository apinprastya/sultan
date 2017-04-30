#ifndef SOCKET_GLOBAL_H
#define SOCKET_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SOCKET_LIBRARY_STATIC
    #define SCOKETSHARED_EXPORT
#else
    #if defined(SOCKET_LIBRARY)
        #define SCOKETSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define SCOKETSHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // SOCKET_GLOBAL_H
