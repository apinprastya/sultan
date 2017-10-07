#include "mainwindowqml.h"
#include <QQmlApplicationEngine>

using namespace LibGUIQML;

MainWindowQML::MainWindowQML(LibG::MessageBus *bus, QObject *parent) :
    QObject(parent),
    mQmlEngine(new QQmlApplicationEngine(this))
{

}

void MainWindowQML::showSplashScreen()
{
    mQmlEngine->load(QUrl("/media/data/Project/Qt/sultan/libguiqml/qml/main.qml"));
}

void MainWindowQML::hideSplashScreen()
{

}

void MainWindowQML::splashShowMessage(const QString &msg)
{

}

void MainWindowQML::showSetting()
{

}

void MainWindowQML::showMainWindow()
{

}

void MainWindowQML::showRestartError(const QString &title, const QString &msg)
{

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
