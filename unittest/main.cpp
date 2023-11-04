#include <QCoreApplication>
#include <QTest>

#include "simple.h"

int main(int argc, char *argv[])
{
    SimpleTest t;

    QTest::qExec( &t );
}
