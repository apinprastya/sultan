#include <QApplication>
#include <QStyleFactory>
#include "easylogging++.h"
#include "core.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));
    Core core;
    core.setup();

    return a.exec();
}
