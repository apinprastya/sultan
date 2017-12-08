#ifndef CONFIGACTION_H
#define CONFIGACTION_H

#include "serveraction.h"

namespace LibServer {

class ConfigAction: public ServerAction
{
public:
    ConfigAction();

protected:
    LibG::Message insertOrUpdate(LibG::Message *msg);
};

}
#endif // CONFIGACTION_H
