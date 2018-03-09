#include "calcTagPos.h"

void calcTagPos::calcKalmanLoose(const int *distance, const locationCoor *sensor, dType T_in,
                                 oneKalmanData &kalmanData, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE,
                                 bool *usedSensor, QVector<QPointF> &iterTrace, QVector<dType> &weight) {
    iterTrace.clear();
    weight.fill(0.f, N);

    dType init_W[MAX_SENSOR];
    for (int i = 0; i < MAX_SENSOR; i++) {
        init_W[i] = 1.f;
    }
    locationCoor z_x_t_meas = {0.f, 0.f, 0.f};
    dType mse = 0.f;

    // constant kalman or complementary filter
    dType T_diff = 1.f;
    locationCoor x_hat_t = kalmanData.x_t_1 + (kalmanData.v_t_1 * T_diff);
    locationCoor v_hat_t = kalmanData.v_t_1;

    calcWeightedTaylor(distance, sensor, kalmanData.x_t_1,
                       N, init_W, nullptr,
                       z_x_t_meas.x, z_x_t_meas.y, mse,
                       usedSensor, iterTrace, weight);

    if (false == kalmanData.isInitialized) {
        kalmanData.x_t = z_x_t_meas;
        kalmanData.v_t = {0.f, 0.f, 0.f};
        kalmanData.isInitialized = true;
    } else {
        kalmanData.x_t = x_hat_t * (1.f - kalmanData.K) + z_x_t_meas * kalmanData.K;
        kalmanData.v_t = v_hat_t * (1.f - kalmanData.K) +
                (kalmanData.x_t - kalmanData.x_t_1) / T_diff * kalmanData.K;
    }

    kalmanData.x_t_1 = kalmanData.x_t;
    kalmanData.v_t_1 = kalmanData.v_t;
    kalmanData.Time = T_in;

    // output
    out_x = kalmanData.x_t.x;
    out_y = kalmanData.x_t.y;
    out_MSE = mse;
}
