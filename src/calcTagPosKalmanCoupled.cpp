#include "calcTagPos.h"
#include <QtMath>

void calcTagPos::calcKalmanCoulped(const int *distance, const locationCoor *sensor, dType T_in,
                                   oneKalmanData &kalmanData, int N, unsigned int type,
                                   dType &out_x, dType &out_y, dType &out_MSE,
                                   bool *usedSensor, QVector<QPointF> &iterTrace,
                                   QVector<dType> &weight, QPointF &out_x_hat) {
    //qDebug() << type;
    iterTrace.clear();
    weight.fill(0.f, N);

    dType init_W[MAX_SENSOR];
    locationCoor z_x_t_meas = {0.f, 0.f, 0.f};
    dType mse = 0.f;

    // constant kalman or complementary filter
    dType T_diff = 1.f;
    locationCoor x_hat_t = kalmanData.x_t_1 + (kalmanData.v_t_1 * T_diff);
    locationCoor v_hat_t = kalmanData.v_t_1;

/* WEIGHT COUPLED ************************************************************/
    for (int i = 0; i < N; i++) {
        init_W[i] = 1.f;

        if (WEIGHT_COUPLED & type) {
            if (kalmanData.isInitialized) {
                dType currDist_hat = calcDistance(x_hat_t, sensor[i]);
                init_W[i] = 1.f / (0.005*qAbs(currDist_hat - distance[i]) + 1.f);
            } else {}
        } else {}
    }
/* GAUSS COUPLED *************************************************************/
    dType *x_hat = nullptr;
    dType pos_hat[2] = {x_hat_t.x, x_hat_t.y};
    if (GAUSS_COUPLED & type) {
        x_hat = kalmanData.isInitialized ? pos_hat : nullptr;
    } else {
        x_hat = nullptr;
    }
/* SMOOTH COUPLED ************************************************************/
    QVector<int> argDistance;
    argDistance.fill(0.f, N);
    for (int i = 0; i < N; i++) {
        argDistance[i] = distance[i];
        if (SMOOTH_COUPLED & type) {
            dType dist_hat = calcDistance(sensor[i], x_hat_t);
            argDistance.append(distance[i] * 0.3f + dist_hat * 0.7f);
        }
    }
/* CALCULATE POSITION ********************************************************/
    calcWeightedTaylor(argDistance.data(), sensor, kalmanData.x_t_1,
                       N, init_W, x_hat,
                       z_x_t_meas.x, z_x_t_meas.y, mse,
                       usedSensor, iterTrace, weight);

    if (false == kalmanData.isInitialized) {
        kalmanData.x_t = z_x_t_meas;
        kalmanData.v_t = {0.f, 0.f, 0.f};
        kalmanData.isInitialized = true;
    } else {
        //if (NONE_COUPLED == type) {
            kalmanData.x_t = z_x_t_meas;
            kalmanData.v_t = {0.f, 0.f, 0.f};
        //} else {
        //    kalmanData.x_t = x_hat_t * (1.f - kalmanData.K) + z_x_t_meas * kalmanData.K;
        //    kalmanData.v_t = v_hat_t * (1.f - kalmanData.K) +
        //            (kalmanData.x_t - kalmanData.x_t_1) / T_diff * kalmanData.K;
        //}
    }

    kalmanData.x_t_1 = kalmanData.x_t;
    kalmanData.v_t_1 = kalmanData.v_t;
    kalmanData.Time = T_in;

    // output
    out_x = kalmanData.x_t.x;
    out_y = kalmanData.x_t.y;
    out_MSE = mse;
    out_x_hat = x_hat_t.toQPointF();
}
