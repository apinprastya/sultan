#include "mainwindowqml.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>

using namespace LibGUIQML;

MainWindowQML::MainWindowQML(LibG::MessageBus *bus, QObject *parent) :
    QObject(parent),
    mQmlEngine(new QQmlApplicationEngine(this))
{

}

void MainWindowQML::showSplashScreen()
{
    mQmlEngine->rootContext()->setContextProperty("CCore", this);
    mQmlEngine->load(QUrl("qrc:/main.qml"));
}

void MainWindowQML::hideSplashScreen()
{

}

void MainWindowQML::splashShowMessage(const QString &msg)
{
    emit signalShowMessage(msg);
}

void MainWindowQML::showSetting()
{

}

void MainWindowQML::showMainWindow()
{
    emit signalShowLogin();
}

void MainWindowQML::showRestartError(const QString &title, const QString &msg)
{
    emit signalShowRestartError(title, msg);
}

void MainWindowQML::guiMessage(int id, const QString &str)
{

}

void MainWindowQML::setSettingSocketOpenClose(std::function<void (const QString &, int)> openCon, std::function<void ()> closeCon)
{

}

void MainWindowQML::messageReceived(LibG::Message *msg)
{

}
