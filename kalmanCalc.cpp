#include "kalmanCalc.h"
#include "calcPos.h"
#include <QtMath>
#include <QDebug>

kalmanCalc::kalmanCalc()
{

}

void kalmanCalc::calcMatrixMulit_KP(const double Kx, const double Kv,
                               const double Pxx_pri_t, const double Pxv_pri_t, const double Pvv_pri_t,
                               double &Pxx_t, double &Pxv_t, double &Pvv_t) {
    /*
     * | 1-Kx , 0| * |Pxx , Pxv|
     * | -Kv  , 1|   |Pxv , Pvv|
    */
    Pxx_t = (1.f - Kx) * Pxx_pri_t;
    Pxv_t = (1.f - Kx) * Pxv_pri_t;
    Pvv_t = -Kv * Pxv_pri_t + Pvv_pri_t;
    //qDebug() << c << B << d << D << c*B << d*D << dD;
}

double kalmanCalc::calcR(locationCoor v_t, locationCoor v_t_1, double reliability) {
    double ans = 0.0f;
    double k = 0.0f;

    double v_mod_square = calcDistanceSquare(v_t, v_t_1);
    double part1 = 1.0f / (1.0f + 3.0*qExp(-v_mod_square+10));
    double part2 = reliability/250.0f;

    ans = k*part1 + (1.0f-k)*part2;
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square
    //         << "part1" << part1 << "part2" << part2 << reliability
    //         << "ans=" << ans;
    double lBound = 0.13;
    ans = ans < lBound ? lBound : ans;
    return ans;
}
double kalmanCalc::calcR(double reliability) {
    double ans = 0.0f;
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
double kalmanCalc::calcR(locationCoor v_t, locationCoor v_t_1) {
    double v_mod_square = calcDistanceSquare(v_t, v_t_1);
    double ans = qExp(-v_mod_square/200.0f);
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square << "ans=" << ans;
    ans = ans < 0.1 ? 0.1 : ans;
    return ans;
}
double kalmanCalc::calcR(QPoint v_t, QPoint v_t_1) {
    return kalmanCalc::calcR(locationCoor(v_t), locationCoor(v_t_1));
}

void kalmanCalc::calcKalmanPosVectorModified(labelInfo *labelPos, labelInfo *labelKalman) {
    if (nullptr == labelPos) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelPos == nullptr";
        return;
    }
    if (nullptr == labelKalman) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelKalman == nullptr";
        return;
    }

    // sample interval
    double delta_t = 0.8f;
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

    double S_x;
    double S_v;
    double Px_t_1;
    double Pv_t_1;
    double Px_t;
    double Pv_t;
    double R_x = 0.0f;
    double R_x_1 = 0.0f;
    double R_v = 0.0f;
    double Q_x = 0.01f;
    //double Q_v = 0.0001f;
    double K_x = 1.0f;
    double K_v = 1.0f;

    labelKalman->Ans.clear();
    x_t_1 = labelPos->Ans[0];
    v_t_1 = {0,0,0};
    Px_t_1 = 0.3f;
    Pv_t_1 = 0.3f;
    R_x = kalmanCalc::calcR(labelPos->Reliability[0]);
    R_x_1 = R_x;
    labelKalman->Ans.append(labelPos->Ans[0]);
    labelKalman->Reliability.append(K_x);
    labelKalman->dataR.append(R_x);
    labelKalman->dataP.append(Px_t_1);

    for(int i = 1; i < labelPos->Ans.count(); i++) {
        // 1. x = Hx + Bu
        x_hat_t = x_t_1 + (v_t_1 * delta_t);
        v_hat_t = v_t_1;
        // 2. P = FPF + Q
        Px_t = Px_t_1 + Pv_t_1 * delta_t * delta_t + Q_x;
        Pv_t = Pv_t_1 /*+ 2 * Q_v / (delta_t * delta_t)*/;
        // 3. y = z - Hx
        z_x_meas = labelPos->Ans[i];
        y_x_tilde = z_x_meas - x_hat_t;
        z_v_meas = (labelPos->Ans[i] - labelPos->Ans[i-1]) / delta_t;
        y_v_tilde = z_v_meas - v_hat_t;
        // 4. S = R + HPH
        R_x = calcR(labelPos->Reliability[i]);
        S_x = R_x + Px_t;
        R_v = (R_x + R_x_1) / (delta_t * delta_t);
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

        labelKalman->Ans.append(x_t);
        labelKalman->AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        labelKalman->Reliability.append(K_x);
        labelKalman->dataR.append(R_x);
        labelKalman->dataP.append(Px_t);
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

void kalmanCalc::calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman) {
    if (nullptr == labelPos) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelPos == nullptr";
        return;
    }
    if (nullptr == labelKalman) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelKalman == nullptr";
        return;
    }

    // sample interval
    double T = 0.8f;
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

    double S;

    //Pxv_pri_t == Pvx_pri_t if they are initialized with same value
    double Pxx_t;
    double Pxv_t;
    double Pvv_t;
    double Pxx_pri_t;
    double Pxv_pri_t;
    double Pvv_pri_t;
    double Pxx_t_1 = 0.1f;
    double Pxv_t_1 = 0.3f;
    double Pvv_t_1 = 0.4f;

    double R_t = 0.0f;
    double Q = 0.01f;
    double Kx = 1.0f;
    double Kv = 1.0f;

    qDebug() << "T=" << T << "Q=" << Q;

    labelKalman->Ans.clear();
    x_t_1 = labelPos->Ans[0];
    v_t_1 = {0,0,0};
    R_t = calcR(labelPos->Reliability[0]);
    labelKalman->Ans.append(labelPos->Ans[0]);
    labelKalman->Reliability.append(Kx);
    labelKalman->dataR.append(R_t);
    labelKalman->dataP.append(Pxx_t_1);

    for(int i = 1; i < labelPos->Ans.count(); i++) {
        // 1. x = Hx + Bu <= H = [1, T]
        x_hat_t = x_t_1 + (v_t_1 * T);
        v_hat_t = v_t_1;
        // *2. P = FPF + Q
        Pxx_pri_t = Pxx_t_1 + 2*T*Pxv_t_1 + T*T*Pvv_t_1 + Q;
        Pxv_pri_t = Pxv_t_1 + T*Pvv_t_1 + Q/T*2.f;
        Pvv_pri_t = Pvv_t_1 + Q/T/T*4.f;
        // 3. y = z - Hx
        z_x_t_meas = labelPos->Ans[i];
        y_x_tilde = z_x_t_meas - x_hat_t;
        // 4. S = R + HPH
        R_t = calcR(labelPos->Reliability[i]);
        S = R_t + Pxx_pri_t;
        // 5. k = PH/S
        Kx = Pxx_pri_t / S;
        Kv = Pxv_pri_t / S;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_x_tilde * Kx;
        v_t = v_hat_t + y_x_tilde * Kv;
        // *7. P = P - KHP
        calcMatrixMulit_KP(Kx, Kv, Pxx_pri_t, Pxv_pri_t, Pvv_pri_t, Pxx_t, Pxv_t, Pvv_t);

        labelKalman->Ans.append(x_t);
        labelKalman->AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        labelKalman->Reliability.append(Kx);
        labelKalman->dataR.append(R_t);
        labelKalman->dataP.append(Pxx_t);

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
