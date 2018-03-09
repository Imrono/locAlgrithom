#include "calcTagPos.h"
#include <QtMath>

void calcTagPos::calcLMedS (const int *distance, const locationCoor *sensor, locationCoor lastPos,
                            int N, dType &out_x, dType &out_y, dType &out_MSE,
                            bool *usedSensor, QVector<QPointF> &iterTrace) {
    // step 1
    int n = N > 4 ? 4 : 3;

    // step 2 ==> subset calc
    int M_tot = 0;
    int M = 0;
    M_tot = combination(n, N);
    M = M_tot > 20 ? 20 : M_tot;
    dType tmpDist[20][10];
    locationCoor tmpSensor[20][10];
    locationCoor pos[20];
    dType midMSE[20];

    // step 3 ==> calc mediums for each subset
    QVector<QVector<int>> idx;
    randomDraw(idx, n, N, M);   // random poll M from C^n_N
    for (int j = 0; j < M; j++) {
        dType tmpMse[10];

        for (int k = 0; k < n; k++) {
            tmpDist[j][k] = distance[idx[j][k]];
            tmpSensor[j][k] = sensor[idx[j][k]];
        }
        dType ans_MSE;
        calcLM(tmpDist[j], tmpSensor[j], lastPos, n, pos[j].x, pos[j].y, ans_MSE, iterTrace);
        pos[j].z = 0.f;

        for (int i = 0; i < n; i++) {
            tmpMse[i] = qPow(calcDistance(pos[j], tmpSensor[j][i]) - tmpDist[j][i], 2);
        }
        for (int k1 = 0; k1 < n; k1++) {
            for(int k2 = k1 + 1; k2 < n; k2++) {
                if(tmpMse[k1] > tmpMse[k2]) {
                    dType tmp = tmpMse[k1];
                    tmpMse[k1] = tmpMse[k2];
                    tmpMse[k2] = tmp;
                }
            }
        }
        midMSE[j] = tmpMse[n/2];
    }

    // step 4 ==> calc m (min med)
    int m = 0;
    dType minMed = midMSE[m];
    for (int j = 0; j < M; j++) {
        if (minMed > midMSE[j]) {
            minMed = midMSE[j];
            m = j;
        }
    }

    // step 5 ==> calc s0
    dType s0 = 1.8426f * (1 + 5.f/(N-2)) * qSqrt(minMed);

    // step 6 ==> pick used sensor
    dType lamda = 1.f;
    dType lastDist[10];
    locationCoor lastSensor[10];
    int nUsed = 0;
    for (int i = 0; i < N; i++) {
        if (qAbs(calcDistance(pos[m], sensor[i]) - distance[i]) / s0 < lamda) {
            lastDist[nUsed] = distance[i];
            lastSensor[nUsed] = sensor[i];

            usedSensor[nUsed] = true;

            nUsed ++;
        } else {
            usedSensor[nUsed] = false;
        }
    }

    // step 7 ==> finial pos calc
    calcLM(lastDist, lastSensor, lastPos, nUsed, out_x, out_y, out_MSE, iterTrace);
}

void calcTagPos::calcLM (const dType *distance, const locationCoor *sensor, locationCoor lastPos,
                         int N, dType &out_x, dType &out_y, dType &out_MSE, QVector<QPointF> &iterTrace) {
    iterTrace.clear();
    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
    dType *tmp_B_taylor = new dType[N];
    dType **tmp_A_taylor = new dType* [N];
    for (int i = 0; i < N; i++) {
        tmp_A_taylor[i] = new dType[2];
    }

    dType tmpD;
    dType dist[MAX_SENSOR];
    for (int i = 0; i < N; i++) {
        dist[i] = distance[i];
    }

    /**********************************************************************/
    // initial point
    if (qAbs(lastPos.x) > MY_EPS && qAbs(lastPos.y) > MY_EPS) {
        X[0] = lastPos.x;
        X[1] = lastPos.y;
    } else {
        dType **tmpA = new dType *[N];
        dType *tmpB = new dType [N];
        for (int i = 0; i < N; i++) {
            tmpA[i] = new dType[3];
            tmpA[i][0] = -2.f*sensor[i].x;
            tmpA[i][1] = -2.f*sensor[i].y;
            tmpA[i][2] = 1.f;
            tmpB[i] = (qPow(distance[i], 2) - qPow(sensor[i].x, 2)	- qPow(sensor[i].y, 2));
        }
        leastSquare_ARM(tmpA, tmpB, X, N, 3, 0.f);

        for (int i = 0; i < N; i++) {
            delete[]tmpA[i];
        }
        delete []tmpA;
        delete []tmpB;
    }

    X[2] = 0.f;
    dType mse = 0.f;
    dType mseKeep = 0.f;
    mseKeep = calcDistanceMSE(distance, X, sensor, N);
    iterTrace.append(QPointF(X[0], X[1]));

    /**********************************************************************/
    // Levenberg-Marquardt Method
    dType mu = .3f;
    // iteration
    int k_max = 20;
    int k = 0;
    dType eps1 = 0.002f;
    dType eps2 = 4.f;
    dType eps3 = 10000.f;

    while (k++ < k_max) {
        dType X0[2];    //Taylor series expansion at x0 point
        X0[0] = X[0]; X0[1] = X[1];
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD =  qSqrt(qPow(X0[0] - sensor[i].x, 2) + qPow(X0[1] - sensor[i].y, 2));
            tmp_A_taylor[i][0] = ((X0[0] - sensor[i].x) / tmpD);
            tmp_A_taylor[i][1] = ((X0[1] - sensor[i].y) / tmpD);
            tmp_B_taylor[i] =    (distance[i] - tmpD);
        }

        leastSquare_ARM(tmp_A_taylor, tmp_B_taylor, dX, N, 2, mu);

        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(distance, X_new, sensor, N);

        X[0] = X_new[0];
        X[1] = X_new[1];

        iterTrace.append(QPointF(X[0], X[1]));

        if (mse < eps3
         || qSqrt(dX[0] * dX[0] + dX[1] * dX[1]) < eps2
         || qAbs(mse - mseKeep) < eps1 * eps3) {
            break;
        }

        mseKeep = mse;
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = mse;

    if (nullptr != tmp_A_taylor) {
        for (int i = 0; i < N; i++) {
            delete []tmp_A_taylor[i];
        }
        delete []tmp_A_taylor;
        tmp_A_taylor = nullptr;
    }
    if (nullptr == tmp_B_taylor)    delete []tmp_B_taylor;
    tmp_B_taylor = nullptr;
}
