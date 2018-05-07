#include "mainwindowqml.h"
#include "presenter/loginpresenter.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

using namespace LibGUIQML;

static MainWindowQML *sInstance = nullptr;

MainWindowQML::MainWindowQML(LibG::MessageBus *bus, QObject *parent) :
    QObject(parent),
    mQmlEngine(new QQmlApplicationEngine(this))
{
    setMessageBus(bus);
    QQuickStyle::setStyle("Material");
    //qRegisterMetaType<class*>("class*");
    //rootContext()->setContextProperty("name", instance);
    qmlRegisterType<LoginPresenter>("com.lekapin.presenter", 0, 1, "LoginPresenter");
    sInstance = this;
}

MainWindowQML *MainWindowQML::instance()
{
    return sInstance;
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
