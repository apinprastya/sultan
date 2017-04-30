#ifndef GUI_GLOBAL_H
#define GUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GUI_LIBRARY_STATIC
    #define GUISHARED_EXPORT
#else
    #if defined(GUI_LIBRARY)
        #define GUISHARED_EXPORT Q_DECL_EXPORT
    #else
        #define GUISHARED_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // GUI_GLOBAL_H
