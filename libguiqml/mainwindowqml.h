#ifndef MAINWINDOW_QML_H
#define MAINWINDOW_QML_H

#include "guiqml_global.h"
#include "abstractsultangui.h"
#include <QObject>

class QQmlApplicationEngine;

namespace LibGUIQML {

class GUIQMLSHARED_EXPORT MainWindowQML : public QObject, public LibG::AbstractSultanGUI
{
    Q_OBJECT
public:
    MainWindowQML(QObject *parent = nullptr);
    void showSplashScreen();
    void hideSplashScreen();
    void splashShowMessage(const QString &msg);
    void showSetting();
    void showMainWindow();
    void showRestartError(const QString &title, const QString &msg);
    void guiMessage(int id, const QString &str);
    void setSettingSocketOpenClose(std::function<void(const QString&, int)> openCon, std::function<void()> closeCon);

private:
    QQmlApplicationEngine *mQmlEngine;
};

}

#endif // MAINWINDOW_QML_H
