#include "calcTagNLOS.h"
#include "matrixop.h"

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

    dType historyDist[4][4] = {0.0f};
    int historyCount = 0;
    dType *B = new dType[4];
    dType *X = new dType[4];
    dType *A_data = new dType[12];
    dType **A = new dType*[4];
    A[0] = A_data;
    A[1] = A_data+3;
    A[2] = A_data+6;
    A[3] = A_data+9;

    /*
    labelDistance tmpDist;

    if (i >= nTime) {
        // 循环4个anchor与tag的测量距离
        for (int j = 0; j < nSensor; j++) {
            // 赋初值
            for (int row = 0; row < 4; row++) {
                A[row][0] = 1;
                A[row][1] = row;
                A[row][2] = row * row;
                B[row] = historyDist[j][row];
                X[row] = 0.0f;
            }
            // 用最小二乘法，计算距离的插值曲线
            leastSquare(A, B, X, 4, 3);
            dType L_hat = X[0] + X[1]*4.f + X[2]*16.f;
            //dType percent = qAbs((L_hat-dist[i].distance[j])/dist[i].distance[j]);
            dType sigma = qSqrt((qPow(historyDist[j][0]-X[0], 2)
                               + qPow(historyDist[j][1]-X[0]-X[1]-X[2], 2)
                               + qPow(historyDist[j][2]-X[0]-2.f*X[1]-4.f*X[2], 2)
                               + qPow(historyDist[j][2]-X[0]-3.f*X[1]-9.f*X[2], 2)
                               + qPow(dist_d->dist[i].distance[j]-L_hat, 2))/5.0f);
            qDebug() << "i" << i << "X=[" << X[0] << X[1] << X[2] << "], L_hat=" << L_hat
                     << "meas=" << dist_d->dist[i].distance[j] << "sigma:" << sigma;

            if (sigma < 25) {
                tmpDist.distance[j] = dist_d->dist[i].distance[j];
            } else {
                tmpDist.distance[j] = L_hat * 0.3 + dist_d->dist[i].distance[j] * 0.7;
            }

            for (int k = 0; k < 3; k++) {
                historyDist[j][k] = historyDist[j][k+1];
            }
            historyDist[j][3] = dType(dist_d->dist[i].distance[j]);
        }
        label->RefinedPoints.append(calcPosFromDistance(tmpDist.distance, 4));
        distRefined.append(tmpDist);
    } else {
        for (int j = 0; j < 4; j++) {
            historyDist[j][historyCount] = dType(dist_d->dist[i].distance[j]);
        }
        historyCount ++;
        label->RefinedPoints.append(calcPosFromDistance(dist_d->dist[i].distance, 4));
        distRefined.append(dist_d->dist[i]);
    }
    */

    delete []A;
    delete []A_data;
    delete []X;
    delete []B;

    return false;
}
bool calcTagNLOS::isMultiPointNLOS(/*IN_OUT*/int *d_t, /*IN*/const int *d_t_1, int nSensor) const {
    if (nSensor == 0) {
        qDebug() << "isMultiPointNLOS: nSensor =" << nSensor;
        return false;
    }

    dType maxDist = 0.0f;
    dType avgDist_noMax = 0.0f;
    for (int i = 0; i < nSensor; i++) {
        dType diffDist = dType(d_t[i]) - dType(d_t_1[i]);
        if (qAbs(maxDist) <= qAbs(diffDist)) {
            maxDist = diffDist;
        }
        avgDist_noMax += diffDist;
    }
    avgDist_noMax = (avgDist_noMax - maxDist) / static_cast<dType>(nSensor-1);
    bool judge = maxDist > avgDist_noMax * multiPointFactor;
    if (judge) {
        refineMultiPointNLOS(d_t, d_t_1, nSensor);
    } else {}

    return judge;
}

/*************************************************************/
void calcTagNLOS::refineWylieNLOS(dType *d_history, dType *d_meas, int nSensor,
                                  dType *d_predict, int *idx, int &num) const {

}
void calcTagNLOS::refineMultiPointNLOS(/*IN_OUT*/int *d_t, /*IN*/const int *d_t_1, int nSensor) const {
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

    d_t[maxIdx] = (d_t_1[maxIdx] + avgDist_noMax) * (1.f - multiPointRatio) // predict
                + (d_t_1[maxIdx] + maxDist) * multiPointRatio;              // measure
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

