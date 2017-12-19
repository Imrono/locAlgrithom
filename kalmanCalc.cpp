#include "kalmanCalc.h"
#include "calcPos.h"
#include <QtMath>
#include <QDebug>

kalmanCalc::kalmanCalc()
{

}

double kalmanCalc::calcSigmaB(locationCoor v_t, locationCoor v_t_1, double reliability) {
    double ans = 0.0f;
    double k = 0.0f;

    double v_mod_square = calcDistanceSquare(v_t, v_t_1);
    double part1 = 1.0f / (1.0f + 3.0*qExp(-v_mod_square+10));
    double part2 = reliability/250.0f;

    ans = k*part1 + (1.0f-k)*part2;
    qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square
             << "part1" << part1 << "part2" << part2 << reliability
             << "ans=" << ans;
    ans = ans < 0.1 ? 0.1 : ans;
    return ans;
}
double kalmanCalc::calcSigmaB(locationCoor v_t, locationCoor v_t_1) {
    double v_mod_square = calcDistanceSquare(v_t, v_t_1);
    double ans = qExp(-v_mod_square/200.0f);
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square << "ans=" << ans;
    ans = ans < 0.1 ? 0.1 : ans;
    return ans;
}
double kalmanCalc::calcSigmaB(QPoint v_t, QPoint v_t_1) {
    return kalmanCalc::calcSigmaB(locationCoor(v_t), locationCoor(v_t_1));
}

void kalmanCalc::calcKalmanPosVector(labelInfo *labelPos, labelInfo *labelKalman, double Q_in) {
    if (nullptr == labelPos) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelPos == nullptr";
        return;
    }
    if (nullptr == labelKalman) {
        qDebug() << "kalmanCalc::calcKalmanPosVector labelKalman == nullptr";
        return;
    }

    locationCoor x_hat_t;
    locationCoor x_t_1;
    locationCoor v_t_1;
    locationCoor v_t_2;
    locationCoor a_t_1;
    double delta_t = 1.f;
    locationCoor x_t_meas;
    locationCoor v_t_meas;
    double sigma_square_A_t_1;  //上一点的可信度，越可信，值越小
    double sigma_square_A_t;    //上一点的可信度，越可信，值越小
    double sigma_square_B;      //测量点的可信度，越可信，值越小
    double k;
    double Q = Q_in;
    //double R;   // is sigmaB2 itself
    locationCoor x_t;

    labelKalman->Ans.clear();
    x_t_1 = labelPos->Ans[0];
    v_t_1 = {0,0,0};
    v_t_2 = {0,0,0};
    a_t_1 = {0,0,0};
    sigma_square_A_t_1 = 0.3f;
    labelKalman->Ans.append(labelPos->Ans[0]);

    for(int i = 1; i < labelPos->Ans.count(); i++) {
        //x_hat_t = x_t_1 + (v_t_1 + a_t_1*delta_t/2.0f) * delta_t;
        x_hat_t = x_t_1 + v_t_1 * delta_t;
        x_t_meas = labelPos->Ans[i];
        v_t_meas = (x_t_meas - x_t_1) / delta_t;
        sigma_square_A_t = delta_t*delta_t * sigma_square_A_t_1 + Q;
        sigma_square_B = kalmanCalc::calcSigmaB(v_t_meas, v_t_1, labelPos->Reliability[i]);
        k = sigma_square_A_t / (sigma_square_A_t + sigma_square_B);
        x_t = x_hat_t * (1 - k) + x_t_meas * k;

        labelKalman->Ans.append(x_t);
        labelKalman->AnsLines.append(QLine{x_t_1.toQPoint(), x_t.toQPoint()});

        /*
        qDebug() << QString("i=%0, sigmaA2_t_1=%1, sigmaA2_t=%2, sigmaB2=%3, k=%4, dist_x=%5, dist_v=%6, Q=%7")
                    .arg(i, 4, 10, QChar('0'))
                    .arg(sigma_square_A_t_1, 8, 'g', 4)
                    .arg(sigma_square_A_t, 8, 'g', 4)
                    .arg(sigma_square_B, 8, 'g', 4)
                    .arg(k, 7, 'g', 4)
                    .arg(calcDistance(kalmanPos[i], pos[i]), 6, 'g', 3)
                    .arg(calcDistance(v_t_1, v_t_meas), 6, 'g', 3)
                    .arg(Q, 6, 'g', 3);
        */

        //a_t_1 = ((x_t - x_t_1) / delta_t - v_t_2) / (2*delta_t);
        v_t_2 = v_t_1;
        v_t_1 = (x_t - x_t_1) / delta_t;
        x_t_1 = x_t;
        a_t_1 = (v_t_1 - v_t_2) / delta_t;
        sigma_square_A_t_1 = sigma_square_A_t * (1 - k);
    }
}
