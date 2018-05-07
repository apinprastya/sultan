#ifndef MAINWINDOW_QML_H
#define MAINWINDOW_QML_H

#include "messagehandler.h"
#include "guiqml_global.h"
#include "abstractsultangui.h"
#include <QObject>

class QQmlApplicationEngine;

namespace LibGUIQML {

class GUIQMLSHARED_EXPORT MainWindowQML : public QObject, public LibG::AbstractSultanGUI, public LibG::MessageHandler
{
    Q_OBJECT
public:
    MainWindowQML(LibG::MessageBus *bus, QObject *parent = nullptr);
    static MainWindowQML *instance();
    void showSplashScreen() override;
    void hideSplashScreen() override;
    void splashShowMessage(const QString &msg) override;
    void showSetting() override;
    void showMainWindow() override;
    void showRestartError(const QString &title, const QString &msg) override;
    void guiMessage(int id, const QString &str) override;
    void setSettingSocketOpenClose(std::function<void(const QString&, int)> openCon, std::function<void()> closeCon) override;

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    QQmlApplicationEngine *mQmlEngine;

signals:
    void signalShowMessage(const QString &msg);
    void signalShowLogin();
    void signalShowRestartError(const QString &title, const QString &msg);
};

}

#endif // MAINWINDOW_QML_H
