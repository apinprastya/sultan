#include "sultanpresenter.h"
#include "mainwindowqml.h"

using namespace LibGUIQML;

SultanPresenter::SultanPresenter(QObject *parent):
    QObject(parent)
{
    setMessageBus(MainWindowQML::instance()->getMessageBus());
}
