#ifndef SULTANPRESENTER_H
#define SULTANPRESENTER_H

#include "messagehandler.h"
#include <QObject>

namespace LibGUIQML {

class SultanPresenter: public QObject, public LibG::MessageHandler
{
    Q_OBJECT
public:
    SultanPresenter(QObject *parent = nullptr);

protected:
    void messageReceived(LibG::Message *msg) override = 0;
};

}

#endif // SULTANPRESENTER_H
