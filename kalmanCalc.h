#ifndef KALMANCALC_H
#define KALMANCALC_H
#include <QVector>
#include "datatype.h"

class kalmanCalc
{
public:
    kalmanCalc();

    QVector<labelDistance> dist;
    QVector<locationCoor>  pos;
    QVector<locationCoor>  kalmanPos;

    void calcKalmanPos();

    static double calcSigmaB(locationCoor v_t, locationCoor v_t_1);
};

#endif // KALMANCALC_H
