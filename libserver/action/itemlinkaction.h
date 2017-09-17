#ifndef ITEMLINKACTION_H
#define ITEMLINKACTION_H

#include "serveraction.h"

namespace LibServer {

class ItemLinkAction : public ServerAction
{
public:
    ItemLinkAction();

protected:
    void selectAndJoin() override;
};

}
#endif // ITEMLINKACTION_H
