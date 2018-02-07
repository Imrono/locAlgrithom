#include "calcTagPos.h"
#include <QtMath>

void calcTagPos::calcLM (const int *distance, const locationCoor *sensor, locationCoor lastPos,
                         dType **A, dType **coA, dType *B, int N,
                         dType **A_taylor, dType *B_taylor, dType *W_taylor,
                         dType &out_x, dType &out_y, dType &out_MSE,
                         bool *usedSensor, QVector<QPointF> &iterTrace) {
    Q_UNUSED(coA);
    Q_UNUSED(A);
    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
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
        for (int i = 0; i < N; i++) {
            tmpA[i] = new dType[3];
            tmpA[i][0] = -2.f*sensor[i].x * W_taylor[i];
            tmpA[i][1] = -2.f*sensor[i].y * W_taylor[i];
            tmpA[i][2] =  1.f                   * W_taylor[i];
            //qDebug() << tmpA[i][0] << tmpA[i][1] << tmpA[i][2];
        }
        for (int i = 0; i < N; i++) {
            B[i] = (qPow(dist[i], 2)
                  - qPow(sensor[i].x, 2)
                  - qPow(sensor[i].y, 2)) * W_taylor[i];
            //qDebug() << B[i];
        }
        //leastSquare(tmpA, B, X, matrixN, 3);
        leastSquare_ARM(tmpA, B, X, N, 3, 0.f);
        //qDebug() << X[0] << X[1] << matrixN;

        for (int i = 0; i < N; i++) {
            delete[]tmpA[i];
        }
        delete[]tmpA;
    }
    X[2] = 0.f;
    dType mse = 0.f;
    dType mseKeep = 0.f;
    mseKeep = calcDistanceMSE(dist, X, sensor, N);
    //qDebug() << QPointF{X[0], X[1]} << mse;
    iterTrace.append(QPointF{X[0], X[1]});
/*
    qDebug() << dist[0] << dist[1] << dist[2] << dist[3] << dist[4] << dist[5] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << idx[4] << idx[5] << ","
             << dist[0] << dist[1] << dist[2] << dist[3] << dist[4] << dist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5]
             << nUnuseableNlos;
*/

    /**********************************************************************/
    // Levenberg-Marquardt Method
    dType mu = .1f;
    // iteration
    int k_max = 15;
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
            A_taylor[i][0] = ((X0[0] - sensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X0[1] - sensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (dist[i] - tmpD)               * W_taylor[i];
        }

        leastSquare_ARM(A_taylor, B_taylor, dX, N, 2, mu);
        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(dist, X_new, sensor, N);

        if (mse < mseKeep) {
            X[0] = X_new[0];
            X[1] = X_new[1];

            if (mse < eps1
             || qSqrt(dX[0] * dX[0] + dX[1] * dX[1]) < eps2
             || qAbs(mse - mseKeep) < eps1 * eps3) {
                break;
            }

            mu *= 0.9f;
            mseKeep = mse;
            iterTrace.append(QPointF{X[0], X[1]});
        } else {
            mu *= 1.1f;
        }
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
}

void calcTagPos::calcLMedS (const int *distance, const locationCoor *sensor, locationCoor lastPos,
                            dType **A, dType **coA, dType *B, int N,
                            dType **A_taylor, dType *B_taylor, dType *W_taylor,
                            dType &out_x, dType &out_y, dType &out_MSE,
                            bool *usedSensor, QVector<QPointF> &iterTrace) {
    // step 1
    int n = N > 4 ? 4 : 3;

    // step 2
    int M_tot = 0;
    int M = 0;
    M_tot = combination(n, N);
    M = M_tot > 20 ? 20 : M_tot;

    // step 3
    QVector<QVector<int>> idx;
    randomDraw(idx, n, N, M);
    for (int i = 0; i < M; i++) {

    }

    // step 4


}
