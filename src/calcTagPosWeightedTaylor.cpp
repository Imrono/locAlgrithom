﻿#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"
extern "C" {
#include "armVersion/calcTagLoc_ARM.h"
}

//dType calcTagPos::lastPos[2] = {0.f, 0.f};

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor,
                                    locationCoor lastPos, int N, dType *init_W, dType *pos_hat,
                                    dType &out_x, dType &out_y, dType &out_MSE,
                                    bool *usedSensor, QVector<QPointF> &iterTrace, QVector<dType> &weight) {
    dType A_data[(MAX_SENSOR+1) * 2];
    dType *A_taylor[MAX_SENSOR+1];
    dType B_taylor [MAX_SENSOR+1];
    dType W_taylor [MAX_SENSOR+1];
    for (int i = 0; i < MAX_SENSOR + 1; i++) {
        A_taylor[i] = &(A_data[i*2]);
        if (nullptr != init_W) {
            W_taylor[i] = init_W[i];
        } else {
            W_taylor[i] = 1.f;
        }
    }

    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
    dType mse = 0.f;
    dType mseKeep = 0.f;

    // clear output parameter
    iterTrace.clear();
    weight.fill(0.f, N);

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
    int nUnuseableNlos = 0;
    int refIdx = (N+1)/2;
    dType midDist = sortedDist[refIdx];
    dType littleDist = sortedDist[refIdx-1];
    for (int i = 0; i < N; i++) {
        dType currDist = sortedDist[i];

        float diffDist = fabsf(currDist-midDist)+1.f;
        if (i < refIdx) {
            usedSensor[idx[i]] = true;
            dType sensorDist = calcDistance(sensor[refIdx-1].toQPointF(), sensor[i].toQPointF());
            if(qAbs(littleDist - sensorDist) * 0.1f < currDist) {
                //W_taylor[i] = 1.f * qPow(diffDist, 0.1);
                W_taylor[i] *= (1.f + 0.01f*diffDist);
            } else {
                W_taylor[i] *= 1.f / qSqrt(diffDist);
            }
        } else {
            if (sortedDist[i] > sortedDist[refIdx] * 1.4f) {
                nUnuseableNlos++;
                usedSensor[idx[i]] = false;
                W_taylor[i] = 0.f;
            } else {
                usedSensor[idx[i]] = true;
                W_taylor[i] *= 1.f / qSqrt(diffDist);
            }
        }
        weight[idx[i]] = W_taylor[i];
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
        dType tmp_A_data[MAX_SENSOR * 3];
        dType *tmpA[MAX_SENSOR];
        for (int i = 0; i < N; i++) {
            tmpA[i] = &(tmp_A_data[i*3]);
        }
        dType tmpB[MAX_SENSOR];
        for (int i = 0; i < N; i++) {
            tmpA[i][0] = -2.f*sortedSensor[i].x * W_taylor[i];
            tmpA[i][1] = -2.f*sortedSensor[i].y * W_taylor[i];
            tmpA[i][2] = 1.f                    * W_taylor[i];
            tmpB[i]    = (qPow(sortedDist[i], 2) - qPow(sortedSensor[i].x, 2)
                          - qPow(sortedSensor[i].y, 2)) * W_taylor[i];
        }
        leastSquare_ARM(tmpA, tmpB, X, matrixN, 3, 0.f);
        //qDebug() << X[0] << X[1] << matrixN;
    }

    X[2] = 0.f;
    mseKeep = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
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
    dType lamda = .3f;
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
		int tmpN = nullptr == pos_hat ? matrixN : matrixN + 1;
        for (int i = 0; i < matrixN; i++) {
            dType tmpD =  qSqrt(qPow(X0[0] - sortedSensor[i].x, 2) + qPow(X0[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X0[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X0[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i]    = (sortedDist[i] - tmpD)               * W_taylor[i];
            //qDebug() << A_taylor[i][0] << A_taylor[i][1] << B_taylor[i];
        }
        if (nullptr != pos_hat) {
			dType W_kalman;
			if (matrixN > 1) W_kalman = (W_taylor[0] + W_taylor[1]) * 0.5f * 0.7f;
			else             W_kalman = W_taylor[0] * 0.3f * 0.7f;
            dType tmpD = qSqrt(qPow(X0[0] - pos_hat[0], 2) + qPow(X0[1] - pos_hat[1], 2) + MY_EPS);
            A_taylor[matrixN][0] = ((X0[0] - pos_hat[0]) / tmpD) * W_kalman;
            A_taylor[matrixN][1] = ((X0[1] - pos_hat[1]) / tmpD) * W_kalman;
            B_taylor[matrixN]    = - tmpD                        * W_kalman;
        }
        //leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda);
        leastSquare_ARM(A_taylor, B_taylor, dX, tmpN, 2, lamda);
        //qDebug() << "X0[0]" << dX[0] << "X0[1]" << dX[1];
        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(sortedDist, X_new, sortedSensor, matrixN);

        /*
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
            //qDebug() << lamda << QPointF{X[0], X[1]} << mse << mseKeep;
            iterTrace.append(QPointF{X[0], X[1]});
        } else {
            mu *= 4.f;
        }
        */
        X[0] = X_new[0];
        X[1] = X_new[1];

        iterTrace.append(QPointF{X[0], X[1]});
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
    out_MSE = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
    //qDebug() << out_x << out_y << k << mse;
}
