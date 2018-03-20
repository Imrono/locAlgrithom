#ifndef _CALCPARAM_H
#define _CALCPARAM_H
#include "dataType.h"
#include "_myheads.h"

#define MACRO_circleR_1 25.0f
#define MACRO_circleR_2 50.0f

struct _calcParam
{
    struct SolverLM {
        static dType lamda0;
        enum LM_TYPE {
            TRUST_REGION,
            LM,
            CONSTANT
        };

        static bool calcIteration(dType mseKeep, dType mse, dType &lamda, unsigned type);

        // iteration
        static int k_max;
        static dType eps1;
        static dType eps2;
        static dType eps3;
    };

    struct WeightedTaylor {
        static int CALC_WEIGHT(const locationCoor *sortedSensor,
                               const dType *sortedDist,
                               const int N,
                               dType *W_taylor); //out
        static dType CALC_KalmanWeight(const dType *sortedWeight,
                                       const int N,
                                       const dType weightCo);
    };

    struct KalmanCoupled {
        static void WEIGHT_COUPLED_weight(const locationCoor x_hat,      //additional info
                                          const locationCoor *sensor,
                                          const int *distance,
                                          const int N,
                                          const bool isInitialized,
                                          dType *sortedWeight);         //out
        static dType GAUSS_COUPLED_weight;
        static dType SMOOTH_COUPLED_K;
        static dType TRAIL_COUPLED_K;
    };

    struct KalmanTrack {
        static dType calcQ();   // Q should low if v is utral high
        static dType calcR(dType reliability, const QString &methodName);
        static dType Pxx_init;
        static dType Pxv_init;
        static dType Pvv_init;
        static locationCoor v_t_init;
    };
};

#endif // _CALCPARAM_H
