#ifndef EMAIL_H
#define EMAIL_H

#include "smtpexports.h"
#include <QObject>
#include <QFutureWatcher>
#include "SmtpMime"

class SMTP_EXPORT Email : public QObject
{
    Q_OBJECT
public:
    Email(QObject *parent = nullptr);
    ~Email();
    inline void setStmpServer(const QString & host = QLatin1String("localhost"), int port = 25, SmtpClient::ConnectionType ct = SmtpClient::TcpConnection){
        mHost = host; mPort = port; mConType = ct;
    }

    inline void setUserPassword(const QString &username, const QString &password) {
        mUsername = username; mPassword = password;
    }

    inline void setSender(const QString &email, const QString &name) {
        mEmailSender = email; mNameSender = name;
    }

    inline void setReceipt(const QString &email) {
        mEmailReceipt = email;
    }

    void sendEmail(const QString body);
    void execSendEmail();
    static void send(const QString &body);

private:
    QString mHost;
    int mPort;
    SmtpClient::ConnectionType mConType;
    QString mUsername;
    QString mPassword;
    QString mEmailSender;
    QString mNameSender;
    QString mEmailReceipt;
    QString mBody;
    QFutureWatcher<void> mWatcher;
};

#endif // EMAIL_H
