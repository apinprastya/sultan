#include "loginpresenter.h"
#include "global_constant.h"
#include "message.h"
#include <QCryptographicHash>
#include <QDebug>

using namespace LibGUIQML;
using namespace LibG;

LoginPresenter::LoginPresenter(QObject *parent) :
    SultanPresenter(parent)
{

}

void LoginPresenter::login(const QString &username, const QString &password)
{
    LibG::Message msg(MSG_TYPE::USER, MSG_COMMAND::LOGIN);
    msg.addData("username", username);
    msg.addData("password", QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Md5).toHex()));
    sendMessage(&msg);
}

void LoginPresenter::messageReceived(LibG::Message *msg)
{
    if(msg->status() == STATUS::ERROR) {
        qDebug() << "ERROR";
    } else {
        qDebug() << "SUCCESS";
    }
}
