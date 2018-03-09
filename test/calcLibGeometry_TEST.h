#ifndef CALCLIBGEOMETRY_TEST_H
#define CALCLIBGEOMETRY_TEST_H
#include <QString>
#include <QtTest>
#include "../src/calcLibGeometry.h"


class calcLibGeometry_TEST : public QObject
{
public:
    Q_OBJECT

public:
    calcLibGeometry_TEST();

private Q_SLOTS:
    void cross2Circle_TEST();
};

#endif // CALCLIBGEOMETRY_TEST_H
