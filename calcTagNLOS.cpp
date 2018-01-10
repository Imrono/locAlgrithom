#include "calcTagNLOS.h"
#include "calcLibMatrixOp.h"
#include "QtMath"

calcTagNLOS::calcTagNLOS()
{
    wylie_B = new dType[wylieN];
    wylie_X = new dType[wylieN];
    wylie_A_data = new dType[wylieN*(wylieOrder+1)];
    wylie_A = new dType*[wylieN];
    wylie_A[0] = wylie_A_data+0*(wylieOrder+1);
    wylie_A[1] = wylie_A_data+1*(wylieOrder+1);
    wylie_A[2] = wylie_A_data+2*(wylieOrder+1);
    wylie_A[3] = wylie_A_data+3*(wylieOrder+1);
    for (int row = 0; row < wylieN; row++) {
        for (int col = 0; col < wylieOrder + 1; col ++) {
            wylie_A[row][col] = qPow(row, col);
        }
    }
    wylieData = new dType[wylieN];
}
calcTagNLOS::~calcTagNLOS()
{
    delete []wylie_A;
    delete []wylie_A_data;
    delete []wylie_X;
    delete []wylie_B;
    delete []wylieData;
}

// POINTS_NLOS
bool calcTagNLOS::pointsPredictNlos(dist4Calc &distCurr, int nSensor,
                                    const QVector<dist4Calc> &distRefined) const {
    if (WYLIE == predictNlos) {
        if (distRefined.count() < wylieN + 1) {
            qDebug() << "distRefined.count() < wylieN + 1" << distRefined.count() << wylieN;
            return false;
        } else {
            dType L_hat;
            bool ans = false;
            for (int i = 0; i < nSensor; i++) {
                for (int j = 0; j < wylieN; j++) {
                    wylieData[j] = distRefined[distRefined.count()-1 - wylieN + j].distance[i];
                }
                int tmp = distCurr.distance[i];
                if (isWylieNLOS(distCurr.distance[i], wylieData, L_hat)) {
                    ans = true;
                } else {}
                qDebug() << distRefined.count() << i << "L_hat" << L_hat << tmp << distCurr.distance[i];
            }
            return ans;
        }
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
// ONE distance
bool calcTagNLOS::isWylieNLOS(int &d_t, dType *d, dType &L_hat) const {
    if (nullptr == d) {
        qDebug() << "isWylieNLOS: nullptr == d";
        return false;
    }

    bool ans = false;
    for (int row = 0; row < wylieN; row++) {
        wylie_B[row] = d[row];
        wylie_X[row] = 0.0f;
    }
    // 用最小二乘法，计算距离的插值曲线
    leastSquare(wylie_A, wylie_B, wylie_X, 4, 3);
    L_hat = wylie_X[0] + wylie_X[1]*4.f + wylie_X[2]*16.f;
    dType sigma = qSqrt((qPow(d[0]-wylie_X[0]-0.f*wylie_X[1]-0.f*wylie_X[2], 2)
                       + qPow(d[1]-wylie_X[0]-1.f*wylie_X[1]-1.f*wylie_X[2], 2)
                       + qPow(d[2]-wylie_X[0]-2.f*wylie_X[1]-4.f*wylie_X[2], 2)
                       + qPow(d[3]-wylie_X[0]-3.f*wylie_X[1]-9.f*wylie_X[2], 2)
                       + qPow(d_t-L_hat, 2))/5.0f);
    qDebug() << "X=[" << wylie_X[0] << wylie_X[1] << wylie_X[2] << "], L_hat=" << L_hat
             << "meas=" << d_t << "sigma:" << sigma;

    if (sigma > wylieThreshold && L_hat <= d_t) {
        refineWylieNLOS(L_hat, d_t);
        ans = true;
    } else {}

    return ans;
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
void calcTagNLOS::refineWylieNLOS(dType L_hat, int &distCurr) const {
    distCurr = L_hat * wylieRatio + distCurr * (1.f - wylieRatio);
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

    //d_t[maxIdx] = (d_t_1[maxIdx] + avgDist_noMax) * (1.f - multiPointRatio) // predict
    //            + (d_t_1[maxIdx] + maxDist) * multiPointRatio;              // measure

    int &d_t_Kalman = d_t[maxIdx];
    int d_t_1_Kalman = d_t_1[maxIdx];
    dType d_hat_t;
    dType d_meas;
    dType y_tilde;
    dType P_t;
    dType P_pri_t;
    static dType P_t_1 = 0.1f;
    dType Q;
    dType R;
    dType S;
    dType K = 1.0f;
    // 1. x = Hx + Bu
    d_hat_t = d_t_1_Kalman + avgDist_noMax;
    // *2. P = FPF + Q
    Q = 0.1f;
    P_pri_t = P_t_1 + Q;
    // 3. y = z - Hx
    d_meas = d_t_Kalman;
    y_tilde = d_meas - d_hat_t;
    // 4. S = R + HPH
    R = qAbs(maxDist) <= qAbs(avgDist_noMax) * 2.f ? 1.2f : 1.8f;
    S = R + P_pri_t;
    // 5. k = PH/S
    K = P_pri_t / S;
    // 6. x = x + Ky
    d_t_Kalman = d_hat_t + y_tilde * K;
    // *7. P = P - KHP
    P_t = P_pri_t - K * P_pri_t;
    // *. update for next
    d_t_1_Kalman = d_t_Kalman;
    P_t_1 = P_t;
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

