#include "kalmanCalc.h"
#include "calcPos.h"
#include <QtMath>
#include <QDebug>

kalmanCalc::kalmanCalc()
{

}

double kalmanCalc::calcSigmaB(locationCoor v_t, locationCoor v_t_1) {
    double v_mod_square = distanceSquare(v_t, v_t_1);
    double ans = qExp(-v_mod_square/200.0f);
    //qDebug() << v_t.toString() << v_t_1.toString() << v_mod_square << "ans=" << ans;
    ans = ans < 0.1 ? 0.1 : ans;
    return ans;
}

void kalmanCalc::calcKalmanPos() {
    locationCoor x_hat_t;
    locationCoor x_t_1;
    locationCoor v_t_1;
    double delta_t = 1;
    locationCoor x_t_meas;
    locationCoor v_t_meas;
    double sigma_square_A;
    double sigma_square_B;
    double k;
    locationCoor x_t;

    x_t_1 = pos[0];
    v_t_1 = {0,0,0};
    kalmanPos.append({0,0,0});

    for(int i = 1; i < pos.count(); i++) {
        x_hat_t = x_t_1 + v_t_1 * delta_t;
        x_t_meas = pos[i];
        v_t_meas = (x_t_meas - x_t_1) / delta_t;
        sigma_square_A = 0.5f;
        sigma_square_B = calcSigmaB(v_t_meas, v_t_1);
        k = sigma_square_A / (sigma_square_A + sigma_square_B);
        x_t = x_hat_t * (1 - k) + x_t_meas * k;
        kalmanPos.append(x_t);

        v_t_1 = (x_t - x_t_1) / delta_t;
        x_t_1 = x_t;
    }
}
