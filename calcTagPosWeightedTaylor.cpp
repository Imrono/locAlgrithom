#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor,
                                    dType **A, dType **coA, dType *B, int N,
                                    dType **A_taylor, dType *B_taylor, dType *W_taylor,
                                    dType &out_x, dType &out_y, dType &out_MSE,
                                    bool *usedSensor, QVector<QPointF> &iterTrace) {
    Q_UNUSED(coA);
    dType X[3] = {0.f};
    dType dX[3] = {0.f};
    dType tmpD = 0.f;
    dType mse = 0.f;
    dType mseLast = 0.f;

    // sort distance
    int idx[MAX_SENSOR] = {0};
    int sortedDist[MAX_SENSOR] = {0};
    locationCoor sortedSensor[MAX_SENSOR];
    sortDistance(distance, idx, N);
    for (int i = 0; i < N; i++) {
        sortedDist[i] = distance[idx[i]];
        sortedSensor[i] = sensor[idx[i]];
    }

    // calculate weight
    dType midDist = sortedDist[2];
    for (int i = 0; i < N; i++) {
        //W_taylor[i] = 1.f / (qPow(qAbs(dType(distance[i])-dType(distance[idx[1]])), 2)+10.f);
        W_taylor[i] = 1.f / qSqrt(qAbs(dType(sortedDist[i])-midDist)+1.f);
    }
    int nUnuseableNlos = 0;
    int refIdx = (N+1)/2;
    for (int i = refIdx+1; i < N; i++) {
        if (sortedDist[i] > sortedDist[refIdx] * 1.4f) {
            nUnuseableNlos++;
            usedSensor[idx[i]] = false;
            W_taylor[i] = 0.f;
        } else {
            usedSensor[idx[i]] = true;
        }
    }
    int matrixN = N - nUnuseableNlos;

    // initial point
    //for (int i = 0; i < N; i++) {
    //    B[i] = qPow(dType(distance[i]), 2) - qPow(sensor[i].x, 2) - qPow(sensor[i].y, 2);
    //}
    //matrixMuti(coA, B, X, 3, N);
    dType **tmpA = new dType *[N];
    for (int i = 0; i < N; i++) {
        tmpA[i] = new dType[3];
        tmpA[i][0] = -2.f*sortedSensor[i].x;
        tmpA[i][1] = -2.f*sortedSensor[i].y;
        tmpA[i][2] = 1.f;
    }
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(sortedDist[i]), 2) - qPow(sortedSensor[i].x, 2) - qPow(sortedSensor[i].y, 2);
    }
    leastSquare(tmpA, B, X, N, 3);
    mse = calcDistanceMSE(distance, X, sensor, N);
    iterTrace.append(QPointF{X[0], X[1]});

    for (int i = 0; i < N; i++) {
        delete []tmpA[i];
    }
    delete []tmpA;
/*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << idx[4] << idx[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5]
             << nUnuseableNlos;
*/
    int count = 0;
    dType lamda = 0.01f;
    dType nu = 30.f;
    while (mse > 5000.f && count++ < 10 && qAbs(mseLast - mse) > 500.f) {
        dType X0[2];//迭代初值，由于有dX，所以要用X0暂存。
        X0[0] = X[0]; X0[1] = X[1];
        mseLast = mse;
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD = qSqrt(qPow(X[0] - sortedSensor[i].x, 2) + qPow(X[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (dType(sortedDist[i]) - tmpD)       * W_taylor[i];
        }

        // Marquardt damping parameter
        leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda);

        X[0] = X0[0] + dX[0];
        X[1] = X0[1] + dX[1];
        dType mseTmp = calcDistanceMSE(distance, X, sensor, matrixN);

        leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda / nu);

        X[0] = X0[0] + dX[0];
        X[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(distance, X, sensor, matrixN);

        if (mse < mseTmp) {
            lamda /= nu;
        } else {
            dType NU = nu;
            int count_2 = 0;
            do {
                leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda * NU);

                X[0] = X0[0] + dX[0];
                X[1] = X0[1] + dX[1];
                mse = calcDistanceMSE(distance, X, sensor, matrixN);
                //iterTrace.append(QPointF{X[0], X[1]});

                mseTmp = mse;
                NU *= nu;
            } while (mse > mseTmp && ++count_2 < 3);
        }
        iterTrace.append(QPointF{X[0], X[1]});
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, matrixN, 3);
    qDebug() << out_x << out_y << count << mse;
}
