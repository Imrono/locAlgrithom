#include "calcTagPos.h"
#include "calcTagTrack.h"
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"
#include <QtMath>

void calcTagPos::calcKalmanLM(const int *distance, const locationCoor *sensor, dType T_in,
                              oneKalmanData &kalmanData,
                              dType **A, dType **coA, dType *B, int N,
                              dType **A_taylor, dType *B_taylor, dType *W_taylor,
                              dType &out_x, dType &out_y, dType &out_MSE,
                              bool *usedSensor, QVector<QPointF> &iterTrace, QVector<dType> &weight) {
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

    // 1. x = Hx + Bu <= H = [1, T]
    dType T_diff = 1.f;
    locationCoor x_hat_t = kalmanData.x_t_1 + (kalmanData.v_t_1 * T_diff);
    locationCoor v_hat_t = kalmanData.v_t_1;

    // calculate weight
    int nUnuseableNlos = 0;
    int refIdx = (N+1)/2;
    dType midDist = sortedDist[refIdx];
    dType littleDist = sortedDist[refIdx-1];
    for (int i = 0; i < N; i++) {
        dType currDist = sortedDist[i];
        if (kalmanData.isInitialized) {
            dType currDist_hat = calcDistance(x_hat_t, sortedSensor[i]);
            W_taylor[i] = 1.f / (0.05*qAbs(currDist_hat - currDist) + 1);
        } else {
            W_taylor[i] = 1.f;
        }

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
                W_taylor[i] *= 0.f;
            } else {
                usedSensor[idx[i]] = true;
                W_taylor[i] *= 1.f / qSqrt(diffDist);
            }
        }
        weight[idx[i]] = W_taylor[i];
    }

    int matrixN = N - nUnuseableNlos;

    /**********************************************************************/
    // initial point
    if (kalmanData.isInitialized) {
        X[0] = kalmanData.x_t.x;
        X[1] = kalmanData.x_t.y;
    } else {
        dType **tmpA = new dType *[N];
        for (int i = 0; i < N; i++) {
            tmpA[i] = new dType[3];
            tmpA[i][0] = -2.f*sortedSensor[i].x * W_taylor[i];
            tmpA[i][1] = -2.f*sortedSensor[i].y * W_taylor[i];
            tmpA[i][2] = 1.f                    * W_taylor[i];
            B[i] = (qPow(sortedDist[i], 2) - qPow(sortedSensor[i].x, 2)	- qPow(sortedSensor[i].y, 2)) * W_taylor[i];
        }
        leastSquare_ARM(tmpA, B, X, matrixN, 3, 0.f);

        for (int i = 0; i < N; i++) {
            delete[]tmpA[i];
        }
        delete[]tmpA;
    }

    X[2] = 0.f;
    dType mse = 0.f;
    dType mseKeep = 0.f;
    mseKeep = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);

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
            tmpD =  qSqrt(qPow(X0[0] - sortedSensor[i].x, 2) + qPow(X0[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X0[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X0[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (sortedDist[i] - tmpD)               * W_taylor[i];
        }

        leastSquare_ARM(A_taylor, B_taylor, dX, matrixN, 2, mu);

        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(sortedDist, X_new, sortedSensor, matrixN);

        X[0] = X_new[0];
        X[1] = X_new[1];

        if (mse < eps3
         || qSqrt(dX[0] * dX[0] + dX[1] * dX[1]) < eps2
         || qAbs(mse - mseKeep) < eps1 * eps3) {
            break;
        }

        mseKeep = mse;
    };

    locationCoor z_x_t_meas = {X[0], X[1], 0.f};
    if (false == kalmanData.isInitialized) {
        kalmanData.x_t = z_x_t_meas;
        kalmanData.v_t = {0.f, 0.f, 0.f};
        kalmanData.isInitialized = true;
	} else {
        kalmanData.x_t = x_hat_t * (1.f - kalmanData.K) + z_x_t_meas * kalmanData.K;
        kalmanData.v_t = v_hat_t * (1.f - kalmanData.K) +
                (kalmanData.x_t - kalmanData.x_t_1) / T_diff * kalmanData.K;
	}

    kalmanData.x_t_1 = kalmanData.x_t;
    kalmanData.v_t_1 = kalmanData.v_t;

    // output
	out_x = X[0];
	out_y = X[1];
    out_MSE = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
}
