#include "kalmanCalc.h"
#include "calcPos.h"
#include <QtMath>
#include <QDebug>

kalmanCalc::kalmanCalc()
{

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
    double ans = (reliability-20)/100.0f;
    double lBound = 0.3;
    ans = ans < lBound ? lBound : ans;
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
    double Q_x = 0.02f;//0.014f;
    double Q_v = 0.001f;
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
        Px_t = Px_t_1 /*+ Pv_t_1 * delta_t * delta_t */+ Q_x;
        Pv_t = Pv_t_1 + 2 * Q_v / (delta_t * delta_t);
        // 3. y = z - Hx
        z_x_meas = labelPos->Ans[i];
        y_x_tilde = z_x_meas - x_hat_t;
        z_v_meas = (labelPos->Ans[i] - labelPos->Ans[i-1]) / delta_t;
        y_v_tilde = z_v_meas - v_hat_t;
        // 4. S = R + HPH
        R_x = kalmanCalc::calcR(labelPos->Reliability[i]);
        S_x = R_x + Px_t;
        R_v = (R_x + R_x_1) / (delta_t * delta_t);
        S_v = R_v + Pv_t;
        // 5. k = PH/S
        K_x = Px_t / S_x;
        K_v = Pv_t / S_v;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_x_tilde * K_x;
        v_t = v_hat_t + y_v_tilde * K_v;
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
