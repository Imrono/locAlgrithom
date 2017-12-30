#include "calcTagNLOS.h"

calcTagNLOS::calcTagNLOS()
{

}

bool calcTagNLOS::isNLOS() {
    QVector<dType> d;
    QVector<dType> d_t, d_t_1;
    bool ans = false;
    if (isWylie)
        //ans = ans || isWylieNLOS(d);
    if (isMultiPoint)
        //ans = ans || isMultiPointNLOS(d_t, d_t_1);
    if (isRes)
        //ans = ans || isResNLOS();
    if (isSumDist)
        //ans = ans || isSumDistNLOS();

    return ans;
}

bool calcTagNLOS::isWylieNLOS(dType **d, int nTime, int nSensor, dType *d_predict) {
    if (0 == nTime || 0 == nSensor) {
        qDebug() << "isWylieNLOS: nTime =" << nTime << ", nSensor =" << nSensor;
        return false;
    }
    return false;
}

bool calcTagNLOS::isMultiPointNLOS(dType *d_t, dType *d_t_1, int n, dType *diff_d) {
    if (n == 0) {
        qDebug() << "isMultiPointNLOS: n =" << n;
        return false;
    }
    return false;
}

bool calcTagNLOS::isResNLOS(dType res) {
    return res > resThreshold;
}

bool calcTagNLOS::isSumDistNLOS(dType sumDist) {
    return sumDist > sumDistThreshold;
}

