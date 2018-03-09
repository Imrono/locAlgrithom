#include "calcLibGeometry_TEST.h"

calcLibGeometry_TEST::calcLibGeometry_TEST()
{

}

void calcLibGeometry_TEST::cross2Circle_TEST() {
    dType x0_1, x0_2, y0_1, y0_2;
    bool isCross;
    calcCross2Circle(0.f, 0.f, 1.f,
                     2.f, 0.f, 1.5f,
                     x0_1, y0_1, x0_2, y0_2, isCross);
    qDebug() << x0_1 << y0_1 << x0_2 << y0_2 << isCross;
    QVERIFY(true);
}
