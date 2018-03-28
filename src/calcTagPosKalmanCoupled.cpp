#include "calcTagPos.h"
#include <QtMath>

void calcTagPos::calcKalmanCoulped(const int *distance, const locationCoor *sensor, dType T_in,
                                   oneKalmanData &kalmanData, int N, unsigned int type,
                                   dType &out_x, dType &out_y, dType &out_MSE,
                                   bool *usedSensor, QVector<QPointF> &iterTrace,
                                   QVector<dType> &weight, QPointF &out_x_hat) {
    //qDebug() << "[@calcTagPos::calcKalmanCoulped]" << type;
    iterTrace.clear();
    weight.fill(0.f, N+1);

    QVector<dType> init_W(MAX_SENSOR, 1.f);
    locationCoor z_x_t_meas = {0.f, 0.f, 0.f};
    dType mse = 0.f;

    // constant kalman or complementary filter
    dType T_diff = 1.f;
    locationCoor x_hat_t = kalmanData.x_t_1 + (kalmanData.v_t_1 * T_diff);
    locationCoor v_hat_t = kalmanData.v_t_1;

/* WEIGHT COUPLED ************************************************************/
    if (WEIGHT_COUPLED & type) {
        _calcParam::KalmanCoupled::WEIGHT_COUPLED_weight(//x_hat_t,
                                                         kalmanData.x_t_1,
                                                         sensor,
                                                         distance,
                                                         N,
                                                         kalmanData.isInitialized,
                                                         init_W.data());
    }
/* GAUSS COUPLED *************************************************************/
    dType *x_hat = nullptr;
    dType pos_hat[3] = {// x_hat_t.x, x_hat_t.y,   // x_hat
                        kalmanData.x_t_1.x, kalmanData.x_t_1.y,
                        // x_hat's weighted coefficient
                        _calcParam::KalmanCoupled::GAUSS_COUPLED_weight};
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
        if (SMOOTH_COUPLED & type) {    // distance filter
            dType dist_hat = calcDistance(sensor[i], x_hat_t);
            argDistance.append(distance[i] * _calcParam::KalmanCoupled::SMOOTH_COUPLED_K
                               + dist_hat * (1.f - _calcParam::KalmanCoupled::SMOOTH_COUPLED_K));
        }
    }

/* CALCULATE POSITION BEGIN **************************************************/
// using weighted Taylor kernel
    calcWeightedTaylor(argDistance.data(), sensor,
                       kalmanData.x_t_1,    //x_hat as lastPos is also reasonable
                       N, init_W.data(), x_hat,
                       z_x_t_meas.x, z_x_t_meas.y, mse,
                       usedSensor, iterTrace, weight);
/**************************************************** CALCULATE POSITION END */

    if (false == kalmanData.isInitialized) {
        kalmanData.x_t = z_x_t_meas;
        kalmanData.v_t = {0.f, 0.f, 0.f};
        kalmanData.K = _calcParam::KalmanCoupled::TRAIL_COUPLED_K;
        kalmanData.isInitialized = true;
    } else {
/* TRAIL COUPLED *************************************************************/
        if (TRAIL_COUPLED & type) {    // which makes point different from ML point
            kalmanData.x_t = x_hat_t * (1.f - kalmanData.K) + z_x_t_meas * kalmanData.K;
            kalmanData.v_t = v_hat_t * (1.f - kalmanData.K) +
                    (kalmanData.x_t - kalmanData.x_t_1) / T_diff * kalmanData.K;
        } else {
            kalmanData.x_t = z_x_t_meas;
            kalmanData.v_t = {0.f, 0.f, 0.f};
        }
    }
/*
    qDebug() << "[@calcTagPos::calcKalmanCoulped]"
             << "x_hat_t=" << x_hat_t.toString()
             << "z_x_t_meas=" << z_x_t_meas.toString()
             << "kalmanData.x_t=" << kalmanData.x_t.toString()
             << "kalmanData.v_t=" << kalmanData.v_t.toString();
*/
    kalmanData.x_t_1 = kalmanData.x_t;
    kalmanData.v_t_1 = kalmanData.v_t;
    kalmanData.Time = T_in;

    // output
    out_x = kalmanData.x_t.x;
    out_y = kalmanData.x_t.y;
    out_MSE = mse;
    out_x_hat = x_hat_t.toQPointF();
}
