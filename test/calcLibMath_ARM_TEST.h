#ifndef CALCLIBMATH_ARM_TEST_H
#define CALCLIBMATH_ARM_TEST_H
#include <QString>
#include <QtTest>
extern "C" {
#include "../src/armVersion/calcTagPosWeightTaylor_ARM.h"
}

class calcLibMath_ARM_TEST : public QObject
{
    Q_OBJECT
public:
    calcLibMath_ARM_TEST();

private Q_SLOTS:
    void ATA_TEST();
    void ATb_TEST();
    void matrix22_inverse_TEST();
    void matrix33_inverse_TEST();
    void leastSquare22_TEST();
    void leastSquare33_TEST();
};

#endif // CALCLIBMATH_ARM_TEST_H
