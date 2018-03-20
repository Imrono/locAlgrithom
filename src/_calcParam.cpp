#include "_calcParam.h"
#include <QtMath>
#include "calcLibGeometry.h"

dType _calcParam::SolverLM::lamda0 = .3f;
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
    for (int i = 0; i < N; i++) {
        dType currDist = sortedDist[i];
        dType diffDist = fabsf(currDist-midDist)+1.f;

        if (i < refIdx) {
            sortedWeight[i] *= (1.f + 0.01f*diffDist);
            int NOK = 0;
            for (int j = 0; j < refIdx; j++) {
                if (i == j) continue;
                dType r1 = sortedDist[i];
                dType r2 = sortedDist[j];
                dType L  = calcDistance(sortedSensor[j].toQPointF(),
                                        sortedSensor[i].toQPointF());
                if (qAbs(r1 - r2) - L <  0.25f * qAbs(r1 - r2)
                       && r1 + r2 - L > -0.25f * (r1 + r2)) {
                } else {
                    NOK ++;
                }
            }
            if (NOK >= 2) {
                sortedWeight[i] *= 0.5f;
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

    int i_max_1, i_max_2;
    dType tmp1 = 0.f, tmp2 = 0.f;
    for (int i = 0; i < N; i++) {
        if (tmp1 < sortedWeight[i]) {
            i_max_1 = i;
            tmp1 = sortedWeight[i];
        } else if (sortedWeight[i] > tmp2) {
            i_max_2 = i;
            tmp2 = sortedWeight[i];
        } else {}
    }
    //W_taylor[i_max_1] *= 2.f;
    //W_taylor[i_max_2] *= 2.f;

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
dType _calcParam::KalmanCoupled::GAUSS_COUPLED_weight = 0.3f;
dType _calcParam::KalmanCoupled::SMOOTH_COUPLED_K = 0.3f;
dType _calcParam::KalmanCoupled::TRAIL_COUPLED_K = 0.6f;
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
            weight[i] = 1.f / (qAbs(currDist_hat - distance[i]) / 100.f + 1.f);
        } else {}
    }
}

/****************************************************************************/
dType _calcParam::KalmanTrack::Pxx_init = .3f;
dType _calcParam::KalmanTrack::Pxv_init = .3f;
dType _calcParam::KalmanTrack::Pvv_init = .3f;
locationCoor _calcParam::KalmanTrack::v_t_init = {0.f, 0.f, 0.f};

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
