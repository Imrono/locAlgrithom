#include <QtTest>
#include "calcLibMatrixOp_TEST.h"
#include "calcLibMath_ARM_TEST.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    calcLibMatrixOp_TEST matrixTest;
    QTest::qExec(&matrixTest);

    calcLibMath_ARM_TEST armMathTest;
    QTest::qExec(&armMathTest);
}
