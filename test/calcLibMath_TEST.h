#ifndef CALCLIBMATH_H
#define CALCLIBMATH_H
#include <QString>
#include <QtTest>
#include "../src/calcLibMath.h"

class calcLibMath_TEST : public QObject
{
    Q_OBJECT

public:
    calcLibMath_TEST();

private Q_SLOTS:
    void randomDraw_TEST();
};

#endif // CALCLIBMATH_H
