#include "simple.h"
#include <QTest>

SimpleTest::SimpleTest(QObject *parent) : QObject(parent) {}

void SimpleTest::testUpper() {
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}
