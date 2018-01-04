#ifndef KALMANCALC_H
#define KALMANCALC_H
#include "_myheads.h"
#include <QVector>
#include "dataType.h"
#include "showStore.h"
#include <QDebug>

inline void calcMatrix22Mulit(const dType a, const dType b, const dType c, const dType d,
                              const dType A, const dType B, const dType C, const dType D,
                              dType &aA, dType &bB, dType &cC, dType &dD) {
    aA = a*A + b*C;
    bB = a*B + b*D;
    cC = c*A + d*C;
    dD = c*B + d*D;
    //qDebug() << c << B << d << D << c*B << d*D << dD;
}

class calcKalman
{
public:
    calcKalman();

    static void calcKalmanPosVectorLite(labelInfo *labelPos, labelInfo *labelKalman);
    static void calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman);
    static void calcMatrixMulit_KP(const dType Kx, const dType Kv,
                                   const dType Pxx_pri_t, const dType Pxv_pri_t, const dType Pvv_pri_t,
                                   dType &Pxx_t, dType &Pxv_t, dType &Pvv_t);

    static dType calcR(dType reliability, CALC_POS_TYPE type = CALC_POS_TYPE::none_type);
    static dType calcR(locationCoor v_t, locationCoor v_t_1, dType reliability);
    static dType calcR(locationCoor v_t, locationCoor v_t_1);
    static dType calcR(QPoint v_t, QPoint v_t_1);
};

#endif // KALMANCALC_H
