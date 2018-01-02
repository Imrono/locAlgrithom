#include "calcTagNLOS.h"

calcTagNLOS::calcTagNLOS()
{}

// POINTS_NLOS
bool calcTagNLOS::pointsPredictNlos(labelDistance &distCurr, int nSensor,
                                    const QVector<labelDistance> &distRefined) const {
    if (WYLIE == predictNlos) {
        return false;
    } else if (MULTI_POINT == predictNlos) {
        int idx_t_1 = distRefined.count() - 1;
        if (0 >= idx_t_1) {
            return false;
        } else {
            return isMultiPointNLOS(distCurr.distance, distRefined[idx_t_1].distance, nSensor);
        }
    } else {
        return false;
    }
}
bool calcTagNLOS::isWylieNLOS(dType **d, int nTime, int nSensor) const {
    if (0 == nTime || 0 == nSensor) {
        qDebug() << "isWylieNLOS: nTime =" << nTime << ", nSensor =" << nSensor;
        return false;
    }
    return false;
}
bool calcTagNLOS::isMultiPointNLOS(/*IN_OUT*/int *d_t, /*IN*/const int *d_t_1, int nSensor) const {
    if (nSensor == 0) {
        qDebug() << "isMultiPointNLOS: nSensor =" << nSensor;
        return false;
    }

    dType maxDist = 0.0f;
    dType avgDist_noMax = 0.0f;
    int maxIdx = -1;
    for (int i = 0; i < nSensor; i++) {
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
        dType ansDist;
        refineMultiPointNLOS(avgDist_noMax, maxDist, d_t_1[maxIdx], ansDist);
        d_t[maxIdx] = ansDist;
    } else {}

    return judge;
}

/*************************************************************/
void calcTagNLOS::refineWylieNLOS(dType *d_history, dType *d_meas, int nSensor,
                                  dType *d_predict, int *idx, int &num) const {

}
void calcTagNLOS::refineMultiPointNLOS(dType avgDist_noMax, dType maxDist, dType d_t_1, dType &d_ans) const {
    //             predict                                    +     measure
    d_ans = (d_t_1 + avgDist_noMax) * (1.f - multiPointRatio) + (d_t_1 + maxDist) * multiPointRatio;
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

