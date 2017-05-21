#ifndef PRINT_GLOBAL_H
#define PRINT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef PRINT_LIBRARY_STATIC
    #define PRINTSHARED_EXPORT
#else
    #if defined(PRINT_LIBRARY)
        #define PRINTSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define PRINTSHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // PRINT_GLOBAL_H
