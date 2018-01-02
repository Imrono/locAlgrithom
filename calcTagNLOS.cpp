#include "calcTagNLOS.h"

calcTagNLOS::calcTagNLOS()
{}

// POINTS_NLOS
bool calcTagNLOS::pointsPredictNlos(const labelDistance &distCurr, int nSensor,
                                    const QVector<labelDistance> &distRefined,
                                    dType *d_predict, int *idx, int &num) const {
    if (WYLIE == predictNlos) {
        return false;
    } else if (MULTI_POINT == predictNlos) {
        int idx_t_1 = distRefined.count() - 1;
        if (0 >= idx_t_1) {
            return false;
        } else {
            return isMultiPointNLOS(distCurr.distance, distRefined[idx_t_1].distance, nSensor,
                                    d_predict, idx, num);
        }
    } else {
        return false;
    }
}
bool calcTagNLOS::isWylieNLOS(dType **d, int nTime, int nSensor,
                              dType *d_predict, int *idx, int &num) const {
    if (0 == nTime || 0 == nSensor) {
        qDebug() << "isWylieNLOS: nTime =" << nTime << ", nSensor =" << nSensor;
        return false;
    }
    return false;
}
bool calcTagNLOS::isMultiPointNLOS(const int *d_t, const int *d_t_1, int nSensor,
                                   dType *d_predict, int *idx, int &num) const {
    if (nSensor == 0) {
        qDebug() << "isMultiPointNLOS: nSensor =" << nSensor;
        return false;
    }
    if (1 != num) {
        qDebug() << "1 != num, num:" << num;
        return false;
    }

    dType maxDist = 0.0f;
    dType avgDist_noMax = 0.0f;
    int maxIdx = -1;
    for (int i = 0; i < nSensor; i++) {
        d_predict[i] = d_t[i];
        dType diffDist = dType(d_t[i]) - dType(d_t_1[i]);
        if (qAbs(maxDist) <= qAbs(diffDist)) {
            maxDist = diffDist;
            maxIdx = i;
        }
        avgDist_noMax += diffDist;
    }
    avgDist_noMax = (avgDist_noMax - maxDist) / static_cast<dType>(nSensor-1);
    bool judge = maxDist > avgDist_noMax * multiPointFactor;
    if (judge) {
        refineMultiPointNLOS(avgDist_noMax, maxDist, d_t_1[maxIdx], *d_predict);
        idx[0] = maxIdx;
    } else {}

    return judge;
}

/*************************************************************/
void calcTagNLOS::refineWylieNLOS(dType *d_history, dType *d_meas, int nSensor,
                                  dType *d_predict, int *idx, int &num) const {

}
void calcTagNLOS::refineMultiPointNLOS(dType avgDist_noMax, dType maxDist, dType d_t_1, dType &d_ans) const {
    d_ans = d_t_1 + maxDist * multiPointRatio + avgDist_noMax * (1.f - multiPointRatio);
}

/*************************************************************/
// POS_PRECISION_NLOS
bool calcTagNLOS::posPrecisionNLOS(dType precision) const {
    if(RESIDUAL == precNlos) {
        return isResNLOS(precision);
    } else if (SUM_DIST == precNlos) {
        return isSumDistNLOS(precision);
    } else {
        return false;
    }
}
bool calcTagNLOS::isResNLOS(dType res) const {
    return res > resThreshold;
}

bool calcTagNLOS::isSumDistNLOS(dType sumDist) const {
    return sumDist > sumDistThreshold;
}

