#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"
extern "C" {
#include "armVersion/calcTagPos_ARM.h"
}

//dType calcTagPos::lastPos[2] = {0.f, 0.f};

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor, locationCoor lastPos,
                                    dType **A, dType **coA, dType *B, int N,
                                    dType **A_taylor, dType *B_taylor, dType *W_taylor,
                                    dType &out_x, dType &out_y, dType &out_MSE,
                                    bool *usedSensor, QVector<QPointF> &iterTrace) {
    Q_UNUSED(coA);
    Q_UNUSED(A);
    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
    dType tmpD = 0.f;
    //dType mse = 0.f;
    //dType mseLast = 0.f;

    // sort distance
    int idx[MAX_SENSOR] = {0};
    dType sortedDist[MAX_SENSOR] = {0.f};
    locationCoor sortedSensor[MAX_SENSOR];
    sortDistance(distance, idx, N);
    for (int i = 0; i < N; i++) {
        sortedDist[i] = distance[idx[i]];
        sortedSensor[i] = sensor[idx[i]];
    }

    // calculate weight
    int refIdx = (N+1)/2;
    dType midDist = sortedDist[refIdx];
    dType littleDist = sortedDist[refIdx-1];
    for (int i = 0; i < N; i++) {
        dType currDist = sortedDist[i];

        float diffDist = fabsf(currDist-midDist)+1.f;
        if (i < refIdx) {
            dType sensorDist = calcDistance(sensor[refIdx-1].toQPointF(), sensor[i].toQPointF());
            if(qAbs(littleDist - sensorDist) * 0.1f < currDist) {
                //W_taylor[i] = 1.f * qPow(diffDist, 0.1);
                W_taylor[i] = 1.f + 0.01f*diffDist;
            } else {
                W_taylor[i] = 1.f / qSqrt(diffDist);
            }
        } else {
            W_taylor[i] = 1.f / qSqrt(diffDist);
        }
    }
    int nUnuseableNlos = 0;
    for (int i = 0; i < N; i++) {
        if (i < refIdx+1) {
            usedSensor[idx[i]] = true;
            continue;
        } else {
            if (sortedDist[i] > sortedDist[refIdx] * 1.4f) {
                nUnuseableNlos++;
                usedSensor[idx[i]] = false;
                W_taylor[i] = 0.f;
            } else {
                usedSensor[idx[i]] = true;
            }
        }
    }
    /*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5] << ","
             << sortedDist[refIdx] << nUnuseableNlos
             << usedSensor[0] << usedSensor[1] << usedSensor[2] << usedSensor[3] << usedSensor[4] << usedSensor[5];
    */
    int matrixN = N - nUnuseableNlos;

    /**********************************************************************/
    // initial point
    if (qAbs(lastPos.x) > MY_EPS && qAbs(lastPos.y) > MY_EPS) {
        X[0] = lastPos.x;
        X[1] = lastPos.y;
    } else {
        dType **tmpA = new dType *[N];
        for (int i = 0; i < N; i++) {
            tmpA[i] = new dType[3];
            tmpA[i][0] = -2.f*sortedSensor[i].x * W_taylor[i];
            tmpA[i][1] = -2.f*sortedSensor[i].y * W_taylor[i];
            tmpA[i][2] =  1.f                   * W_taylor[i];
            //qDebug() << tmpA[i][0] << tmpA[i][1] << tmpA[i][2];
        }
        for (int i = 0; i < N; i++) {
            B[i] = (qPow(sortedDist[i], 2)
                  - qPow(sortedSensor[i].x, 2)
                  - qPow(sortedSensor[i].y, 2)) * W_taylor[i];
            //qDebug() << B[i];
        }
        //leastSquare(tmpA, B, X, matrixN, 3);
        leastSquare_ARM(tmpA, B, X, matrixN, 3, 0.f);
        //qDebug() << X[0] << X[1] << matrixN;

        for (int i = 0; i < N; i++) {
            delete[]tmpA[i];
        }
        delete[]tmpA;
	}
    X[2] = 0.f;
    dType mse = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
    //qDebug() << QPointF{X[0], X[1]} << mse;
    iterTrace.append(QPointF{X[0], X[1]});
/*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << idx[4] << idx[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5]
             << nUnuseableNlos;
*/

    /**********************************************************************/
    // Levenberg-Marquardt Method
    dType mu = .1f;
    dType eps1 = 0.002f;
    dType eps2 = 4.f;
    dType eps3 = 10000.f;

    int k_max = 20;

    // iteration
    int k = 0;
    dType mseKeep = 0.f;
    while (!found && k++ < k_max) {
        mseKeep = mse;
        dType X0[2];    //Taylor series expansion at x0 point
        X0[0] = X[0]; X0[1] = X[1];
        //mseLast = mse;
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD =  qSqrt(qPow(X0[0] - sortedSensor[i].x, 2) + qPow(X0[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X0[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X0[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (sortedDist[i] - tmpD)               * W_taylor[i];
            //qDebug() << A_taylor[i][0] << A_taylor[i][1] << B_taylor[i];
        }

        //leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda);
        leastSquare_ARM(A_taylor, B_taylor, dX, matrixN, 2, mu);
        //qDebug() << "X0[0]" << dX[0] << "X0[1]" << dX[1];
        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(sortedDist, X_new, sortedSensor, matrixN);

        if (mse < mseKeep) {
            X[0] = X_new[0];
            X[1] = X_new[1];
            mu *= 0.9f;
            //qDebug() << lamda << QPointF{X[0], X[1]} << mse << mseLast;
            iterTrace.append(QPointF{X[0], X[1]});
        } else {
            mu *= 1.1f;
        }

        if (mse < eps1
         || qSqrt(dX[0] * dX[0] + dX[1] * dX[1]) < eps2
         || qAbs(mse - mseKeep) < eps1 * eps3) {
            break;
        }
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
    //qDebug() << out_x << out_y << count << mse;
}
