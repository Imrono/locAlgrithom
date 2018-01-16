#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor,
                                    dType **A, dType **coA, dType *B, int N,
                                    dType **A_taylor, dType *B_taylor, dType *W_taylor,
                                    dType &out_x, dType &out_y, dType &out_MSE) {
    dType X[3] = {0.f};
    dType dX[3] = {0.f};
    dType tmpD = 0.f;
    dType mse = 0.f;
    dType mseLast = 0.f;

    // initial point
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i]), 2) - qPow(sensor[i].x, 2) - qPow(sensor[i].y, 2);
    }
    matrixMuti(coA, B, X, 3, N);
    X[2] = qPow(X[0], 2) + qPow(X[1], 2);
    mse = calcMSE(A, B, X, N, 3);

    // sort distance
    int idx[10] = {0};
    int sortedDist[10] = {0};
    locationCoor sortedSensor[10];
    sortDistance(distance, idx, N);
    for (int i = 0; i < N; i++) {
        sortedDist[i] = distance[idx[i]];
        sortedSensor[i] = sensor[idx[i]];
    }

    // calculate weight
    dType midDist = sortedDist[2];
    for (int i = 0; i < N; i++) {
        //W_taylor[i] = 1.f / (qPow(qAbs(dType(distance[i])-dType(distance[idx[1]])), 2)+10.f);
        W_taylor[i] = 1.f / qSqrt(qAbs(dType(sortedDist[i])-midDist)+10.f);
        if (sortedDist[i] > midDist * 2.5f) {
            W_taylor[i] = 0.f;
        }
    }

/*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << ","
             << distance[idx[0]] << distance[idx[1]] << distance[idx[2]] << distance[idx[3]] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5]
             << frontNum << backNum;
*/
    int count = 0;
    dType lamda = 0.1f;
    dType nu = 1.5f;
    while (mse > 10000.f && count++ < 10 && qAbs(mseLast - mse) > 1000.f) {
        mseLast = mse;
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD = qSqrt(qPow(X[0] - sortedSensor[i].x, 2) + qPow(X[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (dType(sortedDist[i]) - tmpD)       * W_taylor[i];
        }

        // Marquardt damping parameter
        leastSquare(A_taylor, B_taylor, dX, N, 2, lamda);

        dType tmpX[3];
        tmpX[0] = X[0]; tmpX[1] = X[1];
        X[0] = tmpX[0] + dX[0];
        X[1] = tmpX[1] + dX[1];
        X[2] = qPow(X[0], 2) + qPow(X[1], 2);

        dType mseTmp = calcMSE(A, B, X, N, 3);

        leastSquare(A_taylor, B_taylor, dX, N, 2, lamda / nu);

        X[0] = tmpX[0] + dX[0];
        X[1] = tmpX[1] + dX[1];
        X[2] = qPow(X[0], 2) + qPow(X[1], 2);

        mse = calcMSE(A, B, X, N, 3);

        if (mse < mseTmp) {
            lamda /= nu;
        } else {
            dType NU = nu;
            int count_2 = 0;
            do {
                leastSquare(A_taylor, B_taylor, dX, N, 2, lamda * NU);

                X[0] = tmpX[0] + dX[0];
                X[1] = tmpX[1] + dX[1];
                X[2] = qPow(X[0], 2) + qPow(X[1], 2);

                mse = calcMSE(A, B, X, N, 3);

                mseTmp = mse;
                NU *= nu;
            } while (mse > mseTmp && ++count_2 < 2);
        }
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, N, 3);
}
