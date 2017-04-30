#include "email.h"
#include <QtConcurrent>
#include <QDebug>

Email::Email(QObject *parent) : QObject(parent)
{   
    connect(&mWatcher, SIGNAL(finished()), SLOT(deleteLater()));
}

Email::~Email()
{
    qDebug() << "Destory";
}

void Email::sendEmail(const QString body)
{
    mBody = body;
    mWatcher.setFuture(QtConcurrent::run(this, &Email::execSendEmail));
}

void Email::execSendEmail()
{
    SmtpClient client(mHost, mPort, mConType);
    client.setUser(mUsername);
    client.setPassword(mPassword);
    MimeMessage msg;
    msg.setSender(new EmailAddress(mEmailSender, mNameSender));
    msg.addRecipient(new EmailAddress(mEmailReceipt));
    msg.setSubject(QLatin1String("WS Sync Error"));
    MimeText mime;
    mime.setText(mBody);
    msg.addPart(&mime);
    if(client.connectToHost()) {
        if(client.login()) {
            client.sendMail(msg);
        }
    }
    client.quit();
}

void Email::send(const QString &body)
{
    Email *email = new Email();
    email->setStmpServer(QLatin1String("smtp.sendgrid.net"), 465, SmtpClient::SslConnection);
    email->setUserPassword(QLatin1String("tandemmail"), QLatin1String("Tandemok9012"));
    email->setSender(QLatin1String("noreply@tandem.co.id"), QLatin1String("No Reply"));
    email->setReceipt(QLatin1String("apin.klas@gmail.com"));
    email->sendEmail(body);
}
