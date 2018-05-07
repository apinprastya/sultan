#ifndef LOGINPRESENTER_H
#define LOGINPRESENTER_H

#include "sultanpresenter.h"
#include <QObject>

namespace LibGUIQML {

class LoginPresenter : public SultanPresenter
{
    Q_OBJECT
public:
    LoginPresenter(QObject *parent = nullptr);
    Q_INVOKABLE void login(const QString &username, const QString &password);

protected:
    void messageReceived(LibG::Message *msg) override;
};

}

#endif // LOGINPRESENTER_H
