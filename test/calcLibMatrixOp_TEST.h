#ifndef CALCLIBMATRIXOP_TEST_H
#define CALCLIBMATRIXOP_TEST_H
#include <QString>
#include <QtTest>
#include "../src/calcLibMatrixOp.h"

class calcLibMatrixOp_TEST : public QObject
{
    Q_OBJECT

public:
    calcLibMatrixOp_TEST();

private Q_SLOTS:
    void matrix22_inverse_TEST();
    void matrix33_inverse_TEST();
};

#endif // CALCLIBMATRIXOP_TEST_H
