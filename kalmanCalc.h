#ifndef KALMANCALC_H
#define KALMANCALC_H
#include <QVector>
#include "datatype.h"
#include "showStore.h"

class kalmanCalc
{
public:
    kalmanCalc();

    static void calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman);

    static double calcR(double reliability);
    static double calcR(locationCoor v_t, locationCoor v_t_1, double reliability);
    static double calcR(locationCoor v_t, locationCoor v_t_1);
    static double calcR(QPoint v_t, QPoint v_t_1);
};

#endif // KALMANCALC_H
