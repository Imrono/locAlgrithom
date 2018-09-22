#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"

//dType calcTagPos::lastPos[2] = {0.f, 0.f};

void calcTagPos::calcWeightedTaylor(const int *distance, const locationCoor *sensor,
                                    locationCoor lastPos, int N, dType *init_W, dType *pos_hat,
                                    dType &out_x, dType &out_y, dType &out_MSE,
                                    bool *usedSensor, QVector<QPointF> &iterTrace, QVector<dType> &weight) {
    dType A_data[(MAX_SENSOR+1) * 2];
    dType *sortedA[MAX_SENSOR+1];
    dType sortedB [MAX_SENSOR+1];
    dType sortedWeight [MAX_SENSOR+1];
    for (int i = 0; i < MAX_SENSOR + 1; i++) {
        sortedA[i] = &(A_data[i*2]);
    }

    dType X[3] = {0.f, 0.f, 0.f};
    dType dX[3] = {0.f, 0.f, 0.f};
    dType mse = 0.f;
    dType mseKeep = 0.f;

    // clear output parameter
    iterTrace.clear();
    weight.fill(0.f, N+1);

    // sort distance
    int idx[MAX_SENSOR]{0};
    dType sortedDist[MAX_SENSOR]{0.f};
    locationCoor sortedSensor[MAX_SENSOR];
    sortDistance(distance, idx, N); // idx: sorted to original
    for (int i = 0; i < N; i++) {
        sortedDist[i] = distance[idx[i]];
        sortedSensor[i] = sensor[idx[i]];
        sortedWeight[i] = init_W ? init_W[idx[i]] : 1.f;
    }

    // calculate weight
    int matrixN = N - _calcParam::WeightedTaylor::CALC_WEIGHT(sortedSensor, sortedDist, N, sortedWeight);
    dType W_kalman = !pos_hat ? 0.f :
            _calcParam::WeightedTaylor::CALC_KalmanWeight(sortedWeight, matrixN, pos_hat[2]);

    for (int i = 0; i < N; i++) {   // record
        usedSensor[idx[i]] = i >= matrixN ? false : true;
        weight[idx[i]] = sortedWeight[i];
    }
    weight[N] = W_kalman;
    /*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << distance[4] << distance[5] << ","
             << sortedDist[0] << sortedDist[1] << sortedDist[2] << sortedDist[3] << sortedDist[4] << sortedDist[5] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3] << W_taylor[4] << W_taylor[5] << ","
             << sortedDist[refIdx] << nUnuseableNlos
             << usedSensor[0] << usedSensor[1] << usedSensor[2] << usedSensor[3] << usedSensor[4] << usedSensor[5];
    */

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
            tmpA[i][0] = -2.f*sortedSensor[i].x * sortedWeight[i];
            tmpA[i][1] = -2.f*sortedSensor[i].y * sortedWeight[i];
            tmpA[i][2] = 1.f                    * sortedWeight[i];
            tmpB[i]    = (qPow(sortedDist[i], 2) - qPow(sortedSensor[i].x, 2)
                          - qPow(sortedSensor[i].y, 2)) * sortedWeight[i];
        }
        leastSquare_ARM(tmpA, tmpB, X, matrixN, 3, 0.f);
        //qDebug() << X[0] << X[1] << matrixN;
    }

    X[2] = 0.f;
    mseKeep = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
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
    dType lamda = _calcParam::SolverLM::lamda0;
    int k = 0;
    while (k++ < _calcParam::SolverLM::k_max) {
        dType X0[2];    //Taylor series expansion at x0 point
        X0[0] = X[0]; X0[1] = X[1];
        // fill the matrix
		int tmpN = nullptr == pos_hat ? matrixN : matrixN + 1;
        for (int i = 0; i < matrixN; i++) {
            dType tmpD =  qSqrt(qPow(X0[0] - sortedSensor[i].x, 2) + qPow(X0[1] - sortedSensor[i].y, 2));
            sortedA[i][0] = ((X0[0] - sortedSensor[i].x) / tmpD) * sortedWeight[i];
            sortedA[i][1] = ((X0[1] - sortedSensor[i].y) / tmpD) * sortedWeight[i];
            sortedB[i]    = (sortedDist[i] - tmpD)               * sortedWeight[i];
        }
        if (nullptr != pos_hat) {
            dType tmpD = qSqrt(qPow(X0[0] - pos_hat[0], 2) + qPow(X0[1] - pos_hat[1], 2) + MY_EPS);
            sortedA[matrixN][0] = ((X0[0] - pos_hat[0]) / tmpD) * W_kalman;
            sortedA[matrixN][1] = ((X0[1] - pos_hat[1]) / tmpD) * W_kalman;
            sortedB[matrixN]    = - tmpD                        * W_kalman;
        }
        //leastSquare(A_taylor, B_taylor, dX, matrixN, 2, lamda);
        leastSquare_ARM(sortedA, sortedB, dX, tmpN, 2, lamda);
        //qDebug() << "X0[0]" << dX[0] << "X0[1]" << dX[1];
        dType X_new[2];
        X_new[0] = X0[0] + dX[0];
        X_new[1] = X0[1] + dX[1];
        mse = calcDistanceMSE(sortedDist, X_new, sortedSensor, matrixN);

        if (_calcParam::SolverLM::calcIteration(mseKeep, mse, lamda,    // update λ
                                                _calcParam::SolverLM::CONSTANT)) {
            X[0] = X_new[0];
            X[1] = X_new[1];

            iterTrace.append(QPointF{X[0], X[1]});
            if (mse < _calcParam::SolverLM::eps3
             || qSqrt(dX[0] * dX[0] + dX[1] * dX[1]) < _calcParam::SolverLM::eps2
             || qAbs(mse - mseKeep) < _calcParam::SolverLM::eps1) {
                break;
            }

            mseKeep = mse;
        }
    };

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcDistanceMSE(sortedDist, X, sortedSensor, matrixN);
}
