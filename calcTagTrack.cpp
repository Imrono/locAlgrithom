#include "calcTagTrack.h"
#include "calcTagPos.h"
#include <QtMath>
#include <QDebug>

calcTagTrack::calcTagTrack() {

}

void calcTagTrack::calcMatrixMulit_KP(const dType Kx, const dType Kv,
                               const dType Pxx_pri_t, const dType Pxv_pri_t, const dType Pvv_pri_t,
                               dType &Pxx_t, dType &Pxv_t, dType &Pvv_t) {
    /*
     * | 1-Kx , 0| * |Pxx , Pxv|
     * | -Kv  , 1|   |Pxv , Pvv|
    */
    Pxx_t = (1.f - Kx) * Pxx_pri_t;
    Pxv_t = (1.f - Kx) * Pxv_pri_t;
    Pvv_t = -Kv * Pxv_pri_t + Pvv_pri_t;
    //qDebug() << c << B << d << D << c*B << d*D << dD;
}

dType calcTagTrack::calcQ() {//Q should be bigger if v is ultra high
    return 0.01f;
}

dType calcTagTrack::calcR(locationCoor v_t, locationCoor v_t_1, dType reliability) {
    dType ans = 0.0f;
    dType k = 0.0f;

    dType v_mod_square = calcDistanceSquare(v_t, v_t_1);
    dType part1 = 1.0f / (1.0f + 3.0*qExp(-v_mod_square+10));
    dType part2 = reliability/250.0f;

    ans = k*part1 + (1.0f-k)*part2;
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square
    //         << "part1" << part1 << "part2" << part2 << reliability
    //         << "ans=" << ans;
    dType lBound = 0.13;
    ans = ans < lBound ? lBound : ans;
    return ans;
}
dType calcTagTrack::calcR(dType reliability, const QString &methodName) {
    if (METHOD_FULL_CENTROID_STR == methodName) {
        reliability /= 100.f;
    } else if (METHOD_SUB_LS_STR == methodName) {
        reliability /= 100.f;
        //qDebug() << "calcKalman::calcR" << reliability;
    } else if (METHOD_TWO_CENTER_STR == methodName){
    } else if (METHOD_TAYLOR_SERIES_STR == methodName) {
        reliability /= 100.f;
    } else {}

    dType ans = 0.0f;
    if (reliability < 2.5f) {
        ans = 0.05;
    } else if (reliability < 8) {
        ans = 0.2;
    } else if (reliability < 20) {
        ans = 0.5;
    } else if (reliability >= 20) {
        ans = reliability / 40.0f;
    } else {}

    return ans;
}
dType calcTagTrack::calcR(locationCoor v_t, locationCoor v_t_1) {
    dType v_mod_square = calcDistanceSquare(v_t, v_t_1);
    dType ans = qExp(-v_mod_square/200.0f);
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square << "ans=" << ans;
    ans = ans < 0.1 ? 0.1 : ans;
    return ans;
}
dType calcTagTrack::calcR(QPoint v_t, QPoint v_t_1) {
    return calcTagTrack::calcR(locationCoor(v_t), locationCoor(v_t_1));
}

void calcTagTrack::calcOneTrack(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo) {
    if (TRACK_METHOD::TRACK_KALMAN == calcTrackMethod) {
        calcKalmanPosVector(tagMeasInfo, tagKalmanInfo);
    } else if (TRACK_METHOD::TRACK_KALMAN_LITE == calcTrackMethod) {
        calcKalmanPosVectorLite(tagMeasInfo, tagKalmanInfo);
    } else if (TRACK_METHOD::TRACK_KALMAN_INFO == calcTrackMethod) {
        calcKalmanPosVectorInfo(tagMeasInfo, tagKalmanInfo);
    } else {}
}

void calcTagTrack::calcKalmanPosVectorLite(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo) {
    if (0 == tagMeasInfo.Ans.count()) {
        qDebug() << "calcKalman::calcKalmanPosVector number MeasPos = 0";
        return;
    }

    dType T;
    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    // time t
    locationCoor x_t;
    locationCoor v_t;
    // time t-1
    locationCoor x_t_1;
    locationCoor v_t_1;
    // measure
    locationCoor z_x_meas;
    locationCoor z_v_meas;
    locationCoor y_x_tilde;
    locationCoor y_v_tilde;

    dType S_x;
    dType S_v;
    dType Px_t_1;
    dType Pv_t_1;
    dType Px_t;
    dType Pv_t;
    dType R_x = 0.0f;
    dType R_x_1 = 0.0f;
    dType R_v = 0.0f;
    dType Q = calcTagTrack::calcQ();
    //dType Q_v = 0.0001f;
    dType K_x = 1.0f;
    dType K_v = 1.0f;

    qDebug() << "[@calcKalmanPosVectorLite]" << "Q=" << Q;

    tagKalmanInfo.clear();

    x_t_1 = tagMeasInfo.Ans[0];
    v_t_1 = {0,0,0};
    Px_t_1 = 0.3f;
    Pv_t_1 = 0.3f;
    R_x = calcTagTrack::calcR(tagMeasInfo.data[0][0], tagMeasInfo.methodName);
    R_x_1 = R_x;
    tagKalmanInfo.Ans.append(tagMeasInfo.Ans[0]);
    tagKalmanInfo.data[0].append(K_x);
    tagKalmanInfo.data[1].append(R_x);
    tagKalmanInfo.data[2].append(Px_t_1);
    tagKalmanInfo.time.append(tagMeasInfo.time[0]);

    for(int i = 1; i < tagMeasInfo.Ans.count(); i++) {
        tagKalmanInfo.time.append(tagMeasInfo.time[i]);
        T = dType(tagMeasInfo.time[i].toMSecsSinceEpoch() - tagMeasInfo.time[i-1].toMSecsSinceEpoch()) / 1000.f;

        // 1. x = Hx + Bu
        x_hat_t = x_t_1 + (v_t_1 * T);
        v_hat_t = v_t_1;
        // 2. P = FPF + Q
        Px_t = Px_t_1 + Pv_t_1 * T * T + Q;
        Pv_t = Pv_t_1 + Q/T/T*4.f;
        // 3. y = z - Hx
        z_x_meas = tagMeasInfo.Ans[i];
        y_x_tilde = z_x_meas - x_hat_t;
        z_v_meas = (tagMeasInfo.Ans[i] - tagMeasInfo.Ans[i-1]) / T;
        y_v_tilde = z_v_meas - v_hat_t;
        // 4. S = R + HPH
        R_x = calcR(tagMeasInfo.data[0][i], tagMeasInfo.methodName);
        S_x = R_x + Px_t;
        R_v = (R_x + R_x_1) / (T * T);
        S_v = R_v + Pv_t;
        // 5. k = PH/S
        K_x = Px_t / S_x;
        K_v = Pv_t / S_v;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_x_tilde * K_x;
        v_t = v_hat_t + y_v_tilde * K_v;
        //x_t = x_t + y_v_tilde * delta_t * 0.5f;
        // 7. P = P - KHP
        Px_t_1 = Px_t - Px_t * K_x;
        Pv_t_1 = Pv_t - Pv_t * K_v;

        tagKalmanInfo.Ans.append(x_t);
        tagKalmanInfo.AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        tagKalmanInfo.data[0].append(K_x);
        tagKalmanInfo.data[1].append(R_x);
        tagKalmanInfo.data[2].append(Px_t);
        /*
        qDebug() << QString("i=%0, sigmaA2_t_1=%1, P_t_1=%2, P_t=%3, K=%4, dist_x=%5, dist_v=%6, Q=%7")
                    .arg(i, 4, 10, QChar('0'))
                    .arg(P_t_1, 8, 'g', 4)
                    .arg(P_t, 8, 'g', 4)
                    .arg(R, 8, 'g', 4)
                    .arg(K, 7, 'g', 4)
                    .arg(calcDistance(x_t, z_t_meas), 6, 'g', 3)
                    .arg(calcDistance(v_t, v_hat_t), 6, 'g', 3)
                    .arg(Q, 6, 'g', 3);
        */
        // *. update for next
        v_t_1 = v_t;
        x_t_1 = x_t;
        R_x_1 = R_x;
    }
}

void calcTagTrack::calcKalmanPosVector(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo) {
    if (0 == tagMeasInfo.Ans.count()) {
        qDebug() << "calcKalman::calcKalmanPosVector number MeasPos = 0";
        return;
    }

    dType R;
    dType Q = calcTagTrack::calcQ();
    dType T;
    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    // time t
    locationCoor x_t;
    locationCoor v_t;
    // time t-1
    locationCoor x_t_1;
    locationCoor v_t_1;
    // measure
    locationCoor z_x_t_meas;
    locationCoor y_x_tilde;

    dType S;

    //Pxv_pri_t == Pvx_pri_t if they are initialized with same value
    dType Pxx_t;
    dType Pxv_t;
    dType Pvv_t;
    dType Pxx_pri_t;
    dType Pxv_pri_t;
    dType Pvv_pri_t;
    dType Pxx_t_1 = 0.1f;
    dType Pxv_t_1 = 0.3f;
    dType Pvv_t_1 = 0.4f;

    dType Kx = 1.0f;
    dType Kv = 1.0f;

    qDebug() << "[@calcKalmanPosVector]" << "Q=" << Q;

    tagKalmanInfo.clear();

    x_t_1 = tagMeasInfo.Ans[0];
    v_t_1 = {0,0,0};
    R = calcR(tagMeasInfo.data[0][0], tagMeasInfo.methodName);
    tagKalmanInfo.Ans.append(tagMeasInfo.Ans[0]);
    tagKalmanInfo.data[0].append(Kx);
    tagKalmanInfo.data[1].append(R);
    tagKalmanInfo.data[2].append(Pxx_t_1);
    tagKalmanInfo.data[3].append(Pxx_t_1);
    tagKalmanInfo.time.append(tagMeasInfo.time[0]);

    for(int i = 1; i < tagMeasInfo.Ans.count(); i++) {
        tagKalmanInfo.time.append(tagMeasInfo.time[i]);
        T = dType(tagMeasInfo.time[i].toMSecsSinceEpoch() - tagMeasInfo.time[i-1].toMSecsSinceEpoch()) / 1000.f;
        // 1. x = Hx + Bu <= H = [1, T]
        x_hat_t = x_t_1 + (v_t_1 * T);
        v_hat_t = v_t_1;
        // *2. P = FPF + Q
        Pxx_pri_t = Pxx_t_1 + 2.f*T*Pxv_t_1 + T*T*Pvv_t_1 + Q;
        Pxv_pri_t = Pxv_t_1 + T*Pvv_t_1 + Q/T*2.f;
        Pvv_pri_t = Pvv_t_1 + Q/T/T*4.f;
        // 3. y = z - Hx
        z_x_t_meas = tagMeasInfo.Ans[i];
        y_x_tilde = z_x_t_meas - x_hat_t;
        // 4. S = R + HPH
        R = calcR(tagMeasInfo.data[0][i], tagMeasInfo.methodName);
        S = R + Pxx_pri_t;
        // 5. k = PH/S
        Kx = Pxx_pri_t / S;
        Kv = Pxv_pri_t / S;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_x_tilde * Kx;
        v_t = v_hat_t + y_x_tilde * Kv;
        // *7. P = P - KHP
        calcMatrixMulit_KP(Kx, Kv, Pxx_pri_t, Pxv_pri_t, Pvv_pri_t, Pxx_t, Pxv_t, Pvv_t);

        tagKalmanInfo.Ans.append(x_t);
        tagKalmanInfo.AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        tagKalmanInfo.data[0].append(Kx);
        tagKalmanInfo.data[1].append(R);
        tagKalmanInfo.data[2].append(Pxx_t);
        tagKalmanInfo.data[3].append(y_x_tilde.x*y_x_tilde.x + y_x_tilde.y*y_x_tilde.y + y_x_tilde.z*y_x_tilde.z);

        /*
        qDebug() << QString("i=%0, P_t_1=%1, P_t=%2, R_t=%3, Kx=%4, dist_x=%5, dist_v=%6, Q=%7")
                    .arg(i, 4, 10, QChar('0'))
                    .arg(Pxx_t_1, 8, 'g', 4)
                    .arg(Pxx_t, 8, 'g', 4)
                    .arg(R_t, 8, 'g', 4)
                    .arg(Kx, 7, 'g', 4)
                    .arg(calcDistance(x_t, z_x_t_meas), 6, 'g', 3)
                    .arg(calcDistance(v_t, v_hat_t), 6, 'g', 3)
                    .arg(Q_x, 6, 'g', 3);
        */
        /*
        qDebug() << QString("i=%0, Pxx_1=%1, Pvx_1=%2, Pxv_1=%3, Pvv_1=%4,"
                            "Pxx=%5, Pvx=%6, Pxv=%7, Pvv=%8, Kx=%9, Kv=%10")
                    .arg(i, 3, 10, QChar('0'))
                    .arg(Pxx_pri_t, 5, 'g', 2)
                    .arg(Pvx_pri_t, 5, 'g', 2)
                    .arg(Pxv_pri_t, 5, 'g', 2)
                    .arg(Pvv_pri_t, 5, 'g', 2)
                    .arg(Pxx_t, 5, 'g', 2)
                    .arg(Pvx_t, 5, 'g', 2)
                    .arg(Pxv_t, 5, 'g', 2)
                    .arg(Pvv_t, 5, 'g', 2)
                    .arg(Kx, 4, 'g', 2)
                    .arg(Kv, 4, 'g', 2);
        */

        // *. update for next
        v_t_1 = v_t;
        x_t_1 = x_t;
        Pxx_t_1 = Pxx_t;
        Pxv_t_1 = Pxv_t;
        Pvv_t_1 = Pvv_t;
    }
}

void calcTagTrack::calcKalmanPosVectorInfo(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo) {
    if (0 == tagMeasInfo.Ans.count()) {
        qDebug() << "calcKalman::calcKalmanPosVector number MeasPos = 0";
        return;
    }

    dType R = 1.0f;
    dType Qxx = calcTagTrack::calcQ();
    dType Qvv = 0.f;
    dType T;
    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    // time t
    locationCoor x_t;
    locationCoor v_t;
    // time t-1
    locationCoor x_t_1;
    locationCoor v_t_1;
    // measure
    locationCoor z_x_t_meas;
    locationCoor y_x_tilde;

    dType S;

    //Pxv_pri_t == Pvx_pri_t if they are initialized with same value
    dType Pxx_t;
    dType Pxv_t;
    dType Pvv_t;
    dType Pxx_pri_t;
    dType Pxv_pri_t;
    dType Pvv_pri_t;
    dType Pxx_t_1 = 0.1f;
    dType Pxv_t_1 = 0.3f;
    dType Pvv_t_1 = 0.4f;

    dType Kx = 1.0f;
    dType Kv = 1.0f;

    const int numCov = 6;
    dType cov[numCov] = {0.f};

    tagKalmanInfo.clear();

    x_t_1 = tagMeasInfo.Ans[0];
    v_t_1 = {0,0,0};
    R = calcR(tagMeasInfo.data[0][0], tagMeasInfo.methodName);
    tagKalmanInfo.Ans.append(tagMeasInfo.Ans[0]);
    tagKalmanInfo.data[0].append(Kx);
    tagKalmanInfo.data[1].append(R);
    tagKalmanInfo.data[2].append(Pxx_t_1);
    tagKalmanInfo.data[3].append(Pxx_t_1);
    tagKalmanInfo.time.append(tagMeasInfo.time[0]);

    for(int i = 1; i < tagMeasInfo.Ans.count(); i++) {
        tagKalmanInfo.time.append(tagMeasInfo.time[i]);
        T = dType(tagMeasInfo.time[i].toMSecsSinceEpoch() - tagMeasInfo.time[i-1].toMSecsSinceEpoch()) / 1000.f;
        // 1. x = Hx + Bu <= H = [1, T]
        x_hat_t = x_t_1 + (v_t_1 * T);
        v_hat_t = v_t_1;
        z_x_t_meas = tagMeasInfo.Ans[i];
        // {* R, Q}
        dType diff_t = locationCoor::dot(z_x_t_meas - x_hat_t, z_x_t_meas - x_hat_t);
        dType cov_hat = Qxx;
        if (i < numCov) {
            cov[i] = diff_t * diff_t;
        } else {
            cov_hat = 0.f;
            for (int j = 1; j < numCov; j++) {
                cov[j-1] = cov[j];
                cov_hat += cov[j-1];
            }
            cov[numCov-1] = diff_t * diff_t;
            cov_hat += cov[numCov-1];
            cov_hat /= numCov;
        }
        // Q = K * cov_hat * K
        Qxx = cov_hat * Kx * Kx;
        Qvv = cov_hat * Kv * Kv;
        // R = cov_hat + HPH
        if (i >= numCov)
            R = cov_hat - Pxx_t;
        R /= 3.f;// added by test
        qDebug() << i << cov_hat << "v" << locationCoor::dot(v_t, v_t) << Pxx_t_1 << "Q=" << Qxx << Qvv << "R=" << R << "K_t_1=" << Kx;

        // *2. P = FPF + Q
        Pxx_pri_t = Pxx_t_1 + 2.f*T*Pxv_t_1 + T*T*Pvv_t_1 + Qxx;
        Pxv_pri_t = Pxv_t_1 + T*Pvv_t_1;
        Pvv_pri_t = Pvv_t_1 + Qvv;
        // 3. y = z - Hx
        y_x_tilde = z_x_t_meas - x_hat_t;
        // 4. S = R + HPH
        S = R + Pxx_pri_t;
        // 5. k = PH/S
        Kx = Pxx_pri_t / S;
        Kv = Pxv_pri_t / S;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_x_tilde * Kx;
        v_t = v_hat_t + y_x_tilde * Kv;
        // *7. P = P - KHP
        calcMatrixMulit_KP(Kx, Kv, Pxx_pri_t, Pxv_pri_t, Pvv_pri_t, Pxx_t, Pxv_t, Pvv_t);

        tagKalmanInfo.Ans.append(x_t);
        tagKalmanInfo.AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        tagKalmanInfo.data[0].append(Kx);
        tagKalmanInfo.data[1].append(R);
        tagKalmanInfo.data[2].append(Pxx_t);
        tagKalmanInfo.data[3].append(y_x_tilde.x*y_x_tilde.x + y_x_tilde.y*y_x_tilde.y + y_x_tilde.z*y_x_tilde.z);

        // *. update for next
        v_t_1 = v_t;
        x_t_1 = x_t;
        Pxx_t_1 = Pxx_t;
        Pxv_t_1 = Pxv_t;
        Pvv_t_1 = Pvv_t;
    }
}
