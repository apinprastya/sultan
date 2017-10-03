#ifndef GUIQML_GLOBAL_H
#define GUIQML_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GUIQML_LIBRARY_STATIC
    #define GUIQMLSHARED_EXPORT
#else
    #if defined(GUIQML_LIBRARY)
        #define GUIQMLSHARED_EXPORT Q_DECL_EXPORT
    #else
        #if defined(SINGLEBIN)
            #define GUIQMLSHARED_EXPORT
        #else
            #define GUIQMLSHARED_EXPORT Q_DECL_IMPORT
        #endif
    #endif
#endif

#endif // GUIQML_GLOBAL_H
