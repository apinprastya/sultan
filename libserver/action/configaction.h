#ifndef CONFIGACTION_H
#define CONFIGACTION_H

#include "serveraction.h"

namespace LibServer {

class ConfigAction : public ServerAction {
  public:
    ConfigAction();

  protected:
    LibG::Message insertOrUpdate(LibG::Message *msg);
};

} // namespace LibServer
#endif // CONFIGACTION_H
