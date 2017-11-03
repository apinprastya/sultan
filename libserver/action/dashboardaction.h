#ifndef DASHBOARDACTION_H
#define DASHBOARDACTION_H

#include "serveraction.h"

namespace LibServer {

class DashboardAction: public ServerAction
{
public:
    DashboardAction();
    LibG::Message tile(LibG::Message *msg);
    LibG::Message sales(LibG::Message *msg);
    LibG::Message stock(LibG::Message *msg);
    LibG::Message debt(LibG::Message *msg);
};

}
#endif // DASHBOARDACTION_H
