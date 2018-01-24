#include <QtTest>
#include "calcLibMatrixOp_TEST.h"

int main(int argc, char *argv[])
{
    calcLibMatrixOp_TEST matrixTest;
    QTest::qExec(&matrixTest);
}
