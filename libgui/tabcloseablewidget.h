#ifndef TABCLOSEABLEWIDGET_H
#define TABCLOSEABLEWIDGET_H

#include "gui_global.h"

namespace LibGUI {

class GUISHARED_EXPORT TabCloseableWidget {
public:
    virtual ~TabCloseableWidget() {}
    virtual bool requestClose() = 0;
};

}
#endif // TABCLOSEABLEWIDGET_H
