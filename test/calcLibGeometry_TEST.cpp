#include "calcLibGeometry_TEST.h"

calcLibGeometry_TEST::calcLibGeometry_TEST()
{

}

void calcLibGeometry_TEST::cross2Circle_TEST() {
    dType x0_1, x0_2, y0_1, y0_2;
    bool isCross;
    dType x1, y1, x2, y2, d1, d2;

    x1 = 0.f;
    y1 = 0.f;
    d1 = 1.f;
    x2 = 2.f;
    y2 = 0.f;
    d2 = 1.000001f;
    qDebug() << "**FIRST**" << x1 << y1 << d1 << x2 << y2 << d2;
    calcCross2Circle(x1, y1, d1, x2, y2, d2,
                     x0_1, y0_1, x0_2, y0_2, isCross);
    qDebug() << x0_1 << y0_1 << x0_2 << y0_2 << isCross;
    QVERIFY(true == isCross);
    QVERIFY(qAbs(x0_1 - 1.f) < 0.001f && qAbs(y0_1 - 0.f) < 0.001f);
    QVERIFY(qAbs(x0_2 - 1.f) < 0.001f && qAbs(y0_2 - 0.f) < 0.001f);

    x1 = 1.f;
    y1 = 1.f;
    d1 = 1.f;
    x2 = 2.f;
    y2 = 2.f;
    d2 = 1.f;
    qDebug() << "**SECOND**" << x1 << y1 << d1 << x2 << y2 << d2;
    calcCross2Circle(x1, y1, d1, x2, y2, d2,
                     x0_1, y0_1, x0_2, y0_2, isCross);
    qDebug() << x0_1 << y0_1 << x0_2 << y0_2 << isCross;
    QVERIFY(true == isCross);
    QVERIFY(qAbs(x0_1 - 2.f) < 0.001f && qAbs(y0_1 - 1.f) < 0.001f);
    QVERIFY(qAbs(x0_2 - 1.f) < 0.001f && qAbs(y0_2 - 2.f) < 0.001f);
}
