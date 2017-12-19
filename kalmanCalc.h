#ifndef KALMANCALC_H
#define KALMANCALC_H
#include <QVector>
#include "datatype.h"
#include "showStore.h"

class kalmanCalc
{
public:
    kalmanCalc();

    static void calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman, double Q_in = 0.014f);

    static double calcSigmaB(locationCoor v_t, locationCoor v_t_1, double reliability);
    static double calcSigmaB(locationCoor v_t, locationCoor v_t_1);
    static double calcSigmaB(QPoint v_t, QPoint v_t_1);
};

#endif // KALMANCALC_H
