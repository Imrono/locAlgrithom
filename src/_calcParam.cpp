#include "_calcParam.h"
#include <QtMath>
#include "calcLibGeometry.h"
#include "calcLibMath.h"

dType _calcParam::SolverLM::lamda0 = .4f;
// iteration
int _calcParam::SolverLM::k_max = 20;
dType _calcParam::SolverLM::eps1 = 0.2f;
dType _calcParam::SolverLM::eps2 = 4.f;
dType _calcParam::SolverLM::eps3 = 100.f;
bool _calcParam::SolverLM::calcIteration(dType mseKeep, dType mse, dType &lamda, unsigned type) {

    if (TRUST_REGION == type) {
        if (mse < mseKeep) {
            lamda *= 0.9f;
            return true;    // return true if accepted
        } else {
            lamda *= 4.f;
            return false;   // return false if reject
        }
    } else if (CONSTANT == type) {
        return true;
    } else if (LM == type) {
        return true;
    } else {}

    return false;
}

int _calcParam::WeightedTaylor::CALC_WEIGHT(const locationCoor *sortedSensor,
                                            const dType *sortedDist,
                                            const int N,
                                            dType *sortedWeight) {
    int nUnuseableNlos = 0;
    int refIdx = (N+1)/2;
    dType midDist = sortedDist[refIdx];
    dType littleDist = sortedDist[refIdx-1];
    for (int i = 0; i < N; i++) {
        dType currDist = sortedDist[i];
        dType diffDist = fabsf(currDist-midDist)+1.f;

        if (i < refIdx) {
            dType sensorDist = calcDistance(sortedSensor[refIdx-1].toQPointF(), sortedSensor[i].toQPointF());
            if (qAbs(littleDist - sensorDist) * 0.1f < currDist) {
                sortedWeight[i] *= (1.f + 0.01f*diffDist);
            } else {
                sortedWeight[i] *= 1.f / qSqrt(diffDist);
            }
        } else {
            if (sortedDist[i] > sortedDist[refIdx] * 1.4f) {
                nUnuseableNlos++;
                sortedWeight[i] = 0.f;
            } else {
                sortedWeight[i] *= 1.f / qSqrt(diffDist);
            }
        }
    }

    return nUnuseableNlos;
}

dType _calcParam::WeightedTaylor::CALC_KalmanWeight(const dType *sortedWeight,
                                                    const int N,
                                                    const dType weightCo) {
    dType W_kalman = 0.f;
    if (N > 1) W_kalman = (sortedWeight[0] + sortedWeight[1]) * 0.5f * weightCo;
    else       W_kalman = sortedWeight[0] * 0.3f * weightCo;
    return W_kalman;
}

/*****************************************************************************/
dType _calcParam::KalmanCoupled::GAUSS_COUPLED_weight = 0.2f;
dType _calcParam::KalmanCoupled::SMOOTH_COUPLED_K = 0.3f;
dType _calcParam::KalmanCoupled::TRAIL_COUPLED_K = 0.6f;
dType _calcParam::KalmanCoupled::TRAIL_COUPLED_K_v = 0.6f;
void _calcParam::KalmanCoupled::WEIGHT_COUPLED_weight(const locationCoor x_hat,
                                                      const locationCoor *sensor,
                                                      const int *distance,
                                                      const int N,
                                                      const bool isInitialized,
                                                      dType *weight) {
    for (int i = 0; i < N; i++) {
        if (isInitialized) {
            dType currDist_hat = calcDistance(x_hat, sensor[i]);
            // TODO: refine the weight
            dType R = 70.f;
            dType diffDist = qAbs(currDist_hat - distance[i]);

            // calculate method 1 & 2
            dType method_1, method_2;
            if (diffDist < R) {
                method_1 = 1.f;
                method_2 = 1.f;
            } else {
                dType x = qAbs(diffDist - R);
                method_1 = qExp(-x*x/(10000.f));
                method_2 = 1.f / (x / 100.f + 1.f);
            }
            weight[i] = method_1;

            // qDebug() << i << distance[i] << method_1 << method_2 << qAbs(method_2 - method_1);
        } else {}
    }
}

/****************************************************************************/
dType _calcParam::KalmanTrack::Pxx_init = .3f;
dType _calcParam::KalmanTrack::Pxv_init = .3f;
dType _calcParam::KalmanTrack::Pvv_init = .3f;
locationCoor _calcParam::KalmanTrack::v_t_init = {0.f, 0.f, 0.f};
dType _calcParam::KalmanTrack::LowPass_V = 0.7f;

dType _calcParam::KalmanTrack::calcQ() {
    return 0.01f;
}

dType _calcParam::KalmanTrack::calcR(dType reliability/*mse*/, const QString &methodName) {
    if (METHOD_FULL_CENTROID_STR == methodName
     || METHOD_SUB_LS_STR == methodName
     || METHOD_TWO_CENTER_STR == methodName
     || METHOD_TAYLOR_SERIES_STR == methodName) {
        reliability /= 100.f;
    } else if (METHOD_WEIGHTED_TAYLOR_STR == methodName) {
        reliability /= 100.f;
    } else if (METHOD_KALMAN_COUPLED_STR == methodName) {
        reliability /= 50.f;
    } else {}

    dType ans = 0.0f;
    if (reliability < 2.5f) {
        ans = 0.05f;
    } else if (reliability < 8.f) {
        ans = 0.2f;
    } else if (reliability < 20.f) {
        ans = 0.5f;
    } else if (reliability >= 20.f) {
        ans = reliability / 40.0f;
    } else {}

    return ans;
}
