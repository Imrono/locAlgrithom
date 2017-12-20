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
    double ans = reliability/250.0f;
    double lBound = 0.13;
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

    // predict
    locationCoor x_hat_t;
    locationCoor v_hat_t;
    locationCoor a_hat_t;
    // time t
    locationCoor x_t;
    locationCoor v_t;
    locationCoor a_t;
    // time t-1
    locationCoor x_t_1;
    locationCoor v_t_1;
    locationCoor a_t_1;

    double delta_t = 1.f;
    locationCoor z_t_meas;
    locationCoor y_tilde;
    double S;
    double P_t_1;       //上一点的方差（可信度），越可信，值越小
    double P_t;         //这一点的方差（可信度），越可信，值越小
    double R = 0.0f;    //测量方差（可信度），越可信，值越小
    double Q = 0.02f;//0.014f;  //模型方差（可信度），越可信，值越小
    double K = 1.0f;

    labelKalman->Ans.clear();
    x_t_1 = labelPos->Ans[0];
    v_t_1 = {0,0,0};
    a_t_1 = {0,0,0};
    P_t_1 = 0.3f;
    labelKalman->Ans.append(labelPos->Ans[0]);
    labelKalman->Reliability.append(K);
    labelKalman->dataR.append(R);
    labelKalman->dataP.append(P_t_1);

    for(int i = 1; i < labelPos->Ans.count(); i++) {
        // 1. x = Hx + Bu
        x_hat_t = x_t_1 + (v_t_1 * delta_t);
        //x_hat_t = x_t_1 + (v_t_1 * delta_t) + (a_t_1 * delta_t * delta_t / 2.0f);
        v_hat_t = v_t_1;
        a_hat_t = a_t_1;
        // 2. P = FPF + Q
        P_t = delta_t * P_t_1 * delta_t + Q;
        // 3. y = z - Hx
        z_t_meas = labelPos->Ans[i];
        y_tilde = z_t_meas - x_hat_t;
        // 4. S = R + HPH
        R = kalmanCalc::calcR(labelPos->Reliability[i]);
        S = R + P_t;
        // 5. k = PH/S
        K = P_t / S;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        x_t = x_hat_t + y_tilde * K;
        v_t = v_hat_t + y_tilde * K * (1.0f/delta_t);
        a_t = a_hat_t + y_tilde * K * (2.0f/(delta_t*delta_t));
        // 7. P = P - KHP
        P_t_1 = P_t - P_t * K;

        labelKalman->Ans.append(x_t);
        labelKalman->AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});
        labelKalman->Reliability.append(K);
        labelKalman->dataR.append(R);
        labelKalman->dataP.append(P_t);
        /*
        qDebug() << QString("i=%0, sigmaA2_t_1=%1, P_t_1=%2, P_t=%3, K=%4, dist_x=%5, dist_v=%6, Q=%7")
                    .arg(i, 4, 10, QChar('0'))
                    .arg(P_t_1, 8, 'g', 4)
                    .arg(P_t, 8, 'g', 4)
                    .arg(sigma_square_B, 8, 'g', 4)
                    .arg(K, 7, 'g', 4)
                    .arg(calcDistance(kalmanPos[i], pos[i]), 6, 'g', 3)
                    .arg(calcDistance(v_t_1, v_t_meas), 6, 'g', 3)
                    .arg(Q, 6, 'g', 3);
        */
        // *. update for next
        a_t_1 = a_t;
        v_t_1 = v_t;
        x_t_1 = x_t;
    }
}
