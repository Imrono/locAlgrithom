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

void calcTagTrack::calcTrackVector(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo) {
    if (TRACK_METHOD::TRACK_NONE == calcTrackMethod) {
        return;
    }

    int tagId = tagMeasInfo.parentTag->tagId;
    if (!tagsTrackParam.contains(tagId)) {
        trackParams trackParam;
        trackParam.Pxx = _calcParam::KalmanTrack::Pxx_init;
        trackParam.Pxv = _calcParam::KalmanTrack::Pxv_init;
        trackParam.Pvv = _calcParam::KalmanTrack::Pvv_init;
        trackParam.x_t = tagMeasInfo.Ans[0];
        trackParam.v_t = _calcParam::KalmanTrack::v_t_init;
        trackParam.v_t_1 = _calcParam::KalmanTrack::v_t_init;
        trackParam.a_t = _calcParam::KalmanTrack::v_t_init;
        // kalman info only
        trackParam.i = 0;
        trackParam.numCov = 6;
        trackParam.Kx = 1.f;
        trackParam.Kv = 1.f;
        tagsTrackParam.insert(tagId, trackParam);
    }

    for(int i = 0; i < tagMeasInfo.Ans.count(); i++) {
        trackParams &tagTrackParam = tagsTrackParam[tagId];
        tagTrackRecord recParam;    // get extra data for analyze, output only
        dType T = 0.f;
        dType Rx = 0.f;
        if (i == 0) {
            tagKalmanInfo.Ans.append(tagMeasInfo.Ans[0]);
        } else {
            locationCoor z_x_meas = tagMeasInfo.Ans[i];
            T = dType(tagMeasInfo.time[i].toMSecsSinceEpoch()
                    - tagMeasInfo.time[i-1].toMSecsSinceEpoch()) / 1000.f;
            Rx = _calcParam::KalmanTrack::calcR(
                        tagMeasInfo.data[storeMethodInfo::STORED_MSE][i], tagMeasInfo.methodName);
            calcOneTrack(z_x_meas, T, Rx, tagTrackParam, recParam);
            // kalman info only
            tagTrackParam.Kx = recParam.Kx;
            tagTrackParam.Kv = recParam.Kv;

            tagKalmanInfo.Ans.append(tagTrackParam.x_t);
            tagKalmanInfo.AnsLines.append(QLine{tagTrackParam.x_t_1.toQPoint(), tagTrackParam.x_t.toQPoint()});

        }
        tagKalmanInfo.time.append(tagMeasInfo.time[i]);
        tagKalmanInfo.data[storeMethodInfo::STORED_Kx].append(recParam.Kx);
        tagKalmanInfo.data[storeMethodInfo::STORED_Rx].append(Rx);
        tagKalmanInfo.data[storeMethodInfo::STORED_Px].append(tagTrackParam.Pxx);
    }
}

void calcTagTrack::calcOneTrack(const locationCoor &z_x_meas,
                                dType T, dType Rx,
                                trackParams &trackParam,
                                tagTrackRecord &recParam) {
    // 0. update
    trackParam.x_t_1 = trackParam.x_t;
    trackParam.v_t_1 = trackParam.v_t;
    if (TRACK_METHOD::TRACK_KALMAN == calcTrackMethod) {
        calcKalmanPos    (z_x_meas, T, Rx, trackParam, recParam);
    } else if (TRACK_METHOD::TRACK_KALMAN_LITE == calcTrackMethod) {
        calcKalmanPosLite(z_x_meas, T, Rx, trackParam, recParam);
    } else if (TRACK_METHOD::TRACK_KALMAN_INFO == calcTrackMethod) {
        calcKalmanPosInfo(z_x_meas, T, trackParam, recParam);
    } else if (TRACK_METHOD::TRACK_NONE == calcTrackMethod) {
        // cleared but do nothing
    } else {}
}

void calcTagTrack::calcKalmanPos(const locationCoor &z_x_meas,
                                 dType T, dType R,
                                 trackParams &trackParam,
                                 tagTrackRecord &recParam) {
    dType Q = _calcParam::KalmanTrack::calcQ();

    // 1. x = Hx + Bu <= H = [1, T]
    locationCoor x_hat_t = trackParam.x_t + (trackParam.v_t * T);
    locationCoor v_hat_t = trackParam.v_t;
    // *2. P = FPF + Q
    dType Pxx_pri_t = trackParam.Pxx + 2.f*T*trackParam.Pxv + T*T*trackParam.Pvv + Q;
    dType Pxv_pri_t = trackParam.Pxv + T*trackParam.Pvv + Q/T*2.f;
    dType Pvv_pri_t = trackParam.Pvv + Q/T/T*4.f;
    // 3. y = z - Hx
    locationCoor y_x_tilde = z_x_meas - x_hat_t;
    // 4. S = R + HPH
    dType S = R + Pxx_pri_t;
    // 5. k = PH/S
    recParam.Kx = Pxx_pri_t / S;
    recParam.Kv = Pxv_pri_t / S;
    // 6. x = x + Ky <= x = Kx + (1-K)z
    trackParam.x_t = x_hat_t + y_x_tilde * recParam.Kx;
    trackParam.v_t = v_hat_t + y_x_tilde * recParam.Kv;
    // *7. P = P - KHP
    calcMatrixMulit_KP(recParam.Kx, recParam.Kv,
                       Pxx_pri_t, Pxv_pri_t, Pvv_pri_t,
                       trackParam.Pxx, trackParam.Pxv, trackParam.Pvv);
}

void calcTagTrack::calcKalmanPosLite(const locationCoor &z_x_meas,
                                     dType T, dType R_x,
                                     trackParams &trackParam,
                                     tagTrackRecord &recParam) {
    dType R_v;
    dType Q = _calcParam::KalmanTrack::calcQ();
    if (T < 0.001)  T = 0.001f;

    // 1. x = Hx + Bu
    locationCoor x_hat_t = trackParam.x_t_1 + (trackParam.v_t_1 * T);
    locationCoor v_hat_t = trackParam.v_t_1;
    // 2. P = FPF + Q
    dType Px_pri_t = trackParam.Pxx + trackParam.Pvv * T * T + Q;
    dType Pv_pri_t = trackParam.Pvv + Q/T/T*4.f;
    // 3. y = z - Hx
    locationCoor y_x_tilde = z_x_meas - x_hat_t;
    locationCoor z_v_meas = (z_x_meas - trackParam.x_t_1) / T;
    locationCoor y_v_tilde = z_v_meas - v_hat_t;
    // 4. S = R + HPH
    dType S_x = R_x + Px_pri_t;
    R_v = (R_x + trackParam.Pxx) / (T * T);
    dType S_v = R_v + Pv_pri_t;
    // 5. k = PH/S
    recParam.Kx = Px_pri_t / S_x;
    recParam.Kv = Pv_pri_t / S_v;
    // 6. x = x + Ky <= x = Kx + (1-K)z
    trackParam.x_t = x_hat_t + y_x_tilde * recParam.Kx;
    trackParam.v_t = v_hat_t + y_v_tilde * recParam.Kv;
    //x_t = x_t + y_v_tilde * delta_t * 0.5f;
    // 7. P = P - KHP
    trackParam.Pxx = Px_pri_t - Px_pri_t * recParam.Kx;
    trackParam.Pvv = Pv_pri_t - Pv_pri_t * recParam.Kv;

    // TODO: smooth the v, such as using complementary filter (average moving)
    // low pass for velocity by decrease a_t
    locationCoor a_t = (trackParam.v_t - trackParam.v_t_1) / T;
    a_t = a_t * _calcParam::KalmanCoupled::TRAIL_COUPLED_K_v;
    trackParam.v_t = a_t * T + trackParam.v_t_1;
}

void calcTagTrack::calcKalmanPosInfo(const locationCoor &z_x_meas,
                                     dType T,
                                     trackParams &trackParam,
                                     tagTrackRecord &recParam) {
    dType R = 1.0f;
    dType Qxx = 50.f;
    dType Qvv = 5.f;
    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    // measure
    locationCoor y_x_tilde;

    dType S;
    dType Pxx_pri_t;
    dType Pxv_pri_t;
    dType Pvv_pri_t;

    // 1. x = Hx + Bu <= H = [1, T]
    x_hat_t = trackParam.x_t_1 + (trackParam.v_t_1 * T);
    v_hat_t = trackParam.v_t_1;
    // {* R, Q}
    dType diff_t = locationCoor::dot(z_x_meas - x_hat_t, z_x_meas - x_hat_t);
    dType cov_hat = Qxx;
    if (trackParam.i < trackParam.numCov) {
        trackParam.cov[trackParam.i] = diff_t * diff_t;
    } else {
        cov_hat = 0.f;
        for (int j = 1; j < trackParam.numCov; j++) {
            trackParam.cov[j-1] = trackParam.cov[j];
            cov_hat += trackParam.cov[j-1];
        }
        trackParam.cov[trackParam.numCov-1] = diff_t * diff_t;
        cov_hat += trackParam.cov[trackParam.numCov-1];
        cov_hat /= trackParam.numCov;
    }
    // Q = K * cov_hat * K
    Qxx = cov_hat * trackParam.Kx * trackParam.Kx > 0.01f ?
                cov_hat * trackParam.Kx * trackParam.Kx : 0.01f;
    Qvv = cov_hat * trackParam.Kv * trackParam.Kv > 0.01f ?
                cov_hat * trackParam.Kv * trackParam.Kv : 0.01f;
    // R = cov_hat + HPH
    if (trackParam.i >= trackParam.numCov)
        R = cov_hat - trackParam.Pxx;
    R /= 3.f;   // added by test
    //qDebug() << i << cov_hat << "v" << locationCoor::dot(v_t, v_t) << Pxx_t_1 << "Q=" << Qxx << Qvv << "R=" << R << "K_t_1=" << Kx;

    // *2. P = FPF + Q
    Pxx_pri_t = trackParam.Pxx + 2.f*T*trackParam.Pxv + T*T*trackParam.Pvv + Qxx;
    Pxv_pri_t = trackParam.Pxv + T*trackParam.Pvv;
    Pvv_pri_t = trackParam.Pvv + Qvv;
    // 3. y = z - Hx
    y_x_tilde = z_x_meas - x_hat_t;
    // 4. S = R + HPH
    S = R + Pxx_pri_t;
    // 5. k = PH/S
    recParam.Kx = Pxx_pri_t / S;
    recParam.Kv = Pxv_pri_t / S;
    // 6. x = x + Ky <= x = Kx + (1-K)z
    trackParam.x_t = x_hat_t + y_x_tilde * recParam.Kx;
    trackParam.v_t = v_hat_t + y_x_tilde * recParam.Kv;
    // *7. P = P - KHP => perhaps negative value
    calcMatrixMulit_KP(recParam.Kx, recParam.Kv,
                       Pxx_pri_t, Pxv_pri_t, Pvv_pri_t,
                       trackParam.Pxx, trackParam.Pxv, trackParam.Pvv);

    trackParam.Kx = recParam.Kx;
    trackParam.Kv = recParam.Kv;
    trackParam.i++;
}
