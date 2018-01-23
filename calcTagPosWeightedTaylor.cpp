#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"

dType calcTagPos::lastPos[2] = {0.f, 0.f};

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor,
                                    dType **A, dType **coA, dType *B, int N,
                                    dType **A_taylor, dType *B_taylor, dType *W_taylor,
                                    dType &out_x, dType &out_y, dType &out_MSE,
                                    bool *usedSensor, QVector<QPointF> &iterTrace) {
    Q_UNUSED(coA);
    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
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
    int refIdx = (N+1)/2;
    dType midDist = sortedDist[2];
    dType littleDist = sortedDist[1];
    for (int i = 0; i < N; i++) {
        dType currDist = dType(sortedDist[i]);
        if (currDist < midDist
         && currDist > 0.3f * littleDist) {
            W_taylor[i] = 1.f * qPow(qAbs(currDist-midDist)+1.f, 0.1);
        } else {
            W_taylor[i] = 1.f / qSqrt(qAbs(currDist-midDist)+1.f);
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
//    /*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5] << ","
             << sortedDist[refIdx] << nUnuseableNlos
             << usedSensor[0] << usedSensor[1] << usedSensor[2] << usedSensor[3] << usedSensor[4] << usedSensor[5];
//    */
    int matrixN = N - nUnuseableNlos;

    // initial point
	if (lastPos[0] > MY_EPS && lastPos[1] > MY_EPS) {
		X[0] = lastPos[0];
		X[1] = lastPos[1];
	} else {
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
		leastSquare(tmpA, B, X, matrixN, 3);
		
		for (int i = 0; i < N; i++) {
			delete[]tmpA[i];
		}
		delete[]tmpA;
	}
	X[2] = X[0] * X[0] + X[1] * X[1];
	mse = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
    qDebug() << QPointF{X[0], X[1]} << mse;
    iterTrace.append(QPointF{X[0], X[1]});
/*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << idx[4] << idx[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5]
             << nUnuseableNlos;
*/
    int count = 0;
	// Marquardt damping parameter using trust region
    dType minLamda = .5f;
    dType maxLamda = 1.f;
	dType lamda = minLamda;
    dType nu = 1.1f;
    dType mse_pref = 10000.f;
    dType alpha = 1.2f;
    dType beta = 50.f;
    do {
        dType X0[2];    //迭代初值，由于有dX，所以要用X0暂存。
        X0[0] = X[0]; X0[1] = X[1];
        mseLast = mse;
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD = qSqrt(qPow(X[0] - sortedSensor[i].x, 2) + qPow(X[1] - sortedSensor[i].y, 2));
            A_taylor[i][0] = ((X[0] - sortedSensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X[1] - sortedSensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (dType(sortedDist[i]) - tmpD)       * W_taylor[i];
        }


        leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda);

        X[0] = X0[0] + dX[0];
        X[1] = X0[1] + dX[1];
		X[2] = X[0] * X[0] + X[1] * X[1];
        mse = calcDistanceMSE(distance, X, sensor, matrixN);

        if (1 == count % 5) {
            if (mse < mse_pref || mseLast > mse * alpha) {
				if (qAbs(lamda - minLamda) < MY_EPS) {
					// do nothing
				} else {
                    lamda /= nu;
				}
            } else if (mse < mse_pref * beta) {
				// do nothing
			} else {
				if (lamda > maxLamda) {
					// do nothing
				} else {
                    lamda *= nu;
				}
			}
		}
        qDebug() << lamda << QPointF{X[0], X[1]} << mse << mseLast;
        iterTrace.append(QPointF{X[0], X[1]});
    } while (mse > mse_pref && count++ < 40 && qAbs(mseLast - mse) > mse_pref/100.f);

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, matrixN, 3);
	lastPos[0] = X[0];
	lastPos[1] = X[1];
    //qDebug() << out_x << out_y << count << mse;
}
