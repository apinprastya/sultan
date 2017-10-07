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
    void showSplashScreen();
    void hideSplashScreen();
    void splashShowMessage(const QString &msg);
    void showSetting();
    void showMainWindow();
    void showRestartError(const QString &title, const QString &msg);
    void guiMessage(int id, const QString &str);
    void setSettingSocketOpenClose(std::function<void(const QString&, int)> openCon, std::function<void()> closeCon);

protected:
    void messageReceived(LibG::Message *msg) override;

private:
    QQmlApplicationEngine *mQmlEngine;
};

}

#endif // MAINWINDOW_QML_H
