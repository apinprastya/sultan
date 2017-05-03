#include <QApplication>
#include "easylogging++.h"
#include "core.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Core core;
    core.init();

    return a.exec();
}
