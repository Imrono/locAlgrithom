#ifndef KALMANCALC_H
#define KALMANCALC_H
#include <QVector>
#include "datatype.h"
#include "showStore.h"
#include <QDebug>

inline void calcMatrix22Mulit(const double a, const double b, const double c, const double d,
                              const double A, const double B, const double C, const double D,
                              double &aA, double &bB, double &cC, double &dD) {
    aA = a*A + b*C;
    bB = a*B + b*D;
    cC = c*A + d*C;
    dD = c*B + d*D;
    //qDebug() << c << B << d << D << c*B << d*D << dD;
}

class kalmanCalc
{
public:
    kalmanCalc();

    static void calcKalmanPosVectorModified(labelInfo *labelPos, labelInfo *labelKalman);
    static void calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman);
    static void calcMatrixMulit_KP(const double Kx, const double Kv,
                                   const double Pxx_pri_t, const double Pxv_pri_t, const double Pvv_pri_t,
                                   double &Pxx_t, double &Pxv_t, double &Pvv_t);

    static double calcR(double reliability);
    static double calcR(locationCoor v_t, locationCoor v_t_1, double reliability);
    static double calcR(locationCoor v_t, locationCoor v_t_1);
    static double calcR(QPoint v_t, QPoint v_t_1);
};

#endif // KALMANCALC_H
