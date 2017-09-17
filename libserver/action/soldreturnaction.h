#ifndef SOLDRETURNACTION_H
#define SOLDRETURNACTION_H

#include "serveraction.h"

namespace LibServer {

class SoldReturnAction : public ServerAction
{
public:
    SoldReturnAction();
    LibG::Message summary(LibG::Message *msg);

protected:
    void selectAndJoin() override;
    void afterInsert(const QVariantMap &data) override;
    void afterUpdate(const QVariantMap &oldData, const QVariantMap &newData) override;
    void afterDelete(const QVariantMap &oldData) override;
};

}
#endif // SOLDRETURNACTION_H
