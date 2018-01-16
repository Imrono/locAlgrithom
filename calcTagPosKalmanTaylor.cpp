#include "calcTagPos.h"
#include "calcTagTrack.h"
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"
#include <QtMath>

void calcTagPos::calcKalmanTaylor(const int *distance, const locationCoor *sensor, dType T_in,
                                  dType **A, dType **coA, dType *B, int N,
                                  dType **A_taylor, dType *B_taylor, dType *W_taylor,
                                  dType &out_x, dType &out_y, dType &out_MSE) {
    // static parameter
    static dType Pxx_t_1 = 0.1f;
    static dType Pxv_t_1 = 0.3f;
    static dType Pvv_t_1 = 0.4f;
    static locationCoor x_t_1;
    static locationCoor v_t_1;
    static dType T_Keep;

    // variance
    dType Pxx_t;
    dType Pxv_t;
    dType Pvv_t;
    dType Pxx_pri_t;
    dType Pxv_pri_t;
    dType Pvv_pri_t;
    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    // time t
    locationCoor x_t;
    locationCoor v_t;
    // measure
    locationCoor z_x_t_meas;
    locationCoor y_x_tilde;

    dType R;
    dType Q = 0.01;
    dType S;
    dType Kx = 1.0f;
    dType Kv = 1.0f;

    dType T_diff = dType(T_in - T_Keep) / 1000.f;
    // 1. x = Hx + Bu <= H = [1, T]
    x_hat_t = x_t_1 + (v_t_1 * T_diff);
    v_hat_t = v_t_1;


    dType X[3] = {0.f};
    dType dX[3] = {0.f};
    dType tmpD = 0.f;
    dType mse = 0.f;
    dType mseKeep = 0.f;
    dType dMse = 10000.f;

    // calculate weight
    int *idx = new int[N];
    sortDistance(distance, idx, N);

    dType dR_total = 0;
    for (int i = 0; i < N; i++) {
        int dR = qAbs(distance[i] - calcDistance(x_hat_t, sensor[i]));
        dR_total += static_cast<dType>(dR) / N;
        W_taylor[i] = 1.f / (dR + 0.5f);
        //W_taylor[i] = 1.f;
    }
/*
    qDebug() << distance[0] << distance[1] << distance[2] << distance[3] << ","
             << idx[0] << idx[1] << idx[2] << idx[3] << ","
             << distance[idx[0]] << distance[idx[1]] << distance[idx[2]] << distance[idx[3]] << ","
             << W_taylor[0] << W_taylor[1] << W_taylor[2] << W_taylor[3];
*/
    delete []idx;

    // initial iteration
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i]), 2) - qPow(sensor[i].x, 2) - qPow(sensor[i].y, 2);
    }
    matrixMuti(coA, B, X, 3, N);
    X[2] = qPow(X[0], 2) + qPow(X[1], 2);
    mse = calcMSE(A, B, X, N, 3);
    // start position solve iteration
    int count = 0;
    while (mse > 10000.f && count < 3 && dMse > 1000.f) {
        //qDebug() << count << "calcTaylorSeries" << mse << dMse;
        // fill the matrix
        for (int i = 0; i < N; i++) {
            tmpD = qSqrt(qPow(X[0] - sensor[i].x, 2) + qPow(X[1] - sensor[i].y, 2));
            A_taylor[i][0] = ((X[0] - sensor[i].x) / tmpD) * W_taylor[i];
            A_taylor[i][1] = ((X[1] - sensor[i].y) / tmpD) * W_taylor[i];
            B_taylor[i] =    (dType(distance[i]) - tmpD)   * W_taylor[i];
        }

        leastSquare(A_taylor, B_taylor, dX, N, 2);

        X[0] += dX[0];
        X[1] += dX[1];
        X[2] = qPow(X[0], 2) + qPow(X[1], 2);

        mseKeep = mse;
        mse = calcMSE(A, B, X, N, 3);
        dMse = qAbs(mseKeep - mse);

        count ++;
    };

    // *2. P = FPF + Q
    Pxx_pri_t = Pxx_t_1 + 2.f*T_diff*Pxv_t_1 + T_diff*T_diff*Pvv_t_1 + Q;
    Pxv_pri_t = Pxv_t_1 + T_diff*Pvv_t_1 + Q/T_diff*2.f;
    Pvv_pri_t = Pvv_t_1 + Q/T_diff/T_diff*4.f;
    // 3. y = z - Hx
    z_x_t_meas = {X[0], X[1], 0.f};
    y_x_tilde = z_x_t_meas - x_hat_t;
    // 4. S = R + HPH
    R = calcTagTrack::calcR(mse, METHOD_TAYLOR_SERIES_STR);
    S = R + Pxx_pri_t;
    // 5. k = PH/S
    Kx = Pxx_pri_t / S;
    Kv = Pxv_pri_t / S;
    // 6. x = x + Ky <= x = Kx + (1-K)z
    x_t = x_hat_t + y_x_tilde * Kx;
    v_t = v_hat_t + y_x_tilde * Kv;
    // *7. P = P - KHP
    calcTagTrack::calcMatrixMulit_KP(Kx, Kv, Pxx_pri_t, Pxv_pri_t, Pvv_pri_t, Pxx_t, Pxv_t, Pvv_t);

    // *. update for next
    x_t_1 = x_t;
    v_t_1 = v_t;
    Pxx_t_1 = Pxx_t;
    Pxv_t_1 = Pxv_t;
    Pvv_t_1 = Pvv_t;
    T_Keep = T_in;

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, N, 3);
}
