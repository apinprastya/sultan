#ifndef SIMPLE_H
#define SIMPLE_H

#include <QObject>

class SimpleTest : public QObject
{
    Q_OBJECT
public:
    SimpleTest(QObject *parent = 0);

private slots:
    void testUpper();
};

#endif // SIMPLE_H
