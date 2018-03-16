#include "calcTagPos.h"
#include "calcLibGeometry.h"
#include <QtMath>

void calcTagPos::calcBilateration  (const int *distance, const locationCoor *sensor, int N,
                                    dType &out_x, dType &out_y, dType &out_MSE) {
    //qDebug() << "[@calcTagPos::calcBilateration] calcBilateration";
    Q_UNUSED(distance);
    Q_UNUSED(sensor);
    Q_UNUSED(N);
    Q_UNUSED(out_x);
    Q_UNUSED(out_y);
    Q_UNUSED(out_MSE);

    // perform bad, not implement yet
}
