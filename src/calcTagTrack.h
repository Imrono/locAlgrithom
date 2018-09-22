#ifndef KALMANCALC_H
#define KALMANCALC_H
#include "_myheads.h"
#include <QVector>
#include "dataType.h"
#include "showTagModel.h"
#include "calcLibGeometry.h"

inline void calcMatrix22Mulit(const dType a, const dType b, const dType c, const dType d,
                              const dType A, const dType B, const dType C, const dType D,
                              dType &aA, dType &bB, dType &cC, dType &dD) {
    aA = a*A + b*C;
    bB = a*B + b*D;
    cC = c*A + d*C;
    dD = c*B + d*D;
}

struct trackParams {  // used for next point calulate
    dType Pxx;
    dType Pxv;
    dType Pvv;
    locationCoor v_t;
    locationCoor v_t_1;
    locationCoor x_t;
    locationCoor x_t_1;
    locationCoor a_t;
    //kalman info only
    int i{0};
    int numCov{6};
    dType cov[6];
    dType Kx;
    dType Kv;
};

struct tagTrackRecord { // not necessory, just for record and analyze
    dType Kx;
    dType Kv;
    dType Pxx;
    dType Pxv;
    dType Pvv;
    dType R;
    dType T_1;
};

class calcTagTrack
{
public:
    calcTagTrack();

    QMap<int, trackParams> tagsTrackParam;
    void clearParam() {
        tagsTrackParam.clear();
    }

    // ALL IN ONE
    void calcTrackVector(storeMethodInfo &tagMeasInfo, storeMethodInfo &tagKalmanInfo);
    void calcOneTrack(const locationCoor &z_x_meas,
                      dType T, dType Rx,
                      trackParams &tagParam,
                      tagTrackRecord &recParam);
    static void calcKalmanPos    (const locationCoor &z_x_meas,
                                  dType T,  dType R,
                                  trackParams &tagParam,
                                  tagTrackRecord &recParam);
    static void calcKalmanPosLite(const locationCoor &z_x_meas,
                                  dType T, dType Rx,
                                  trackParams &tagParam,
                                  tagTrackRecord &recParam);
    static void calcKalmanPosInfo(const locationCoor &z_x_meas,
                                  dType T,
                                  trackParams &tagParam,
                                  tagTrackRecord &recParam);
    static void calcMovingAverage(const locationCoor &z_x_meas,
                                  dType T,
                                  trackParams &tagParam,
                                  tagTrackRecord &recParam);

    static void calcMatrixMulit_KP(const dType Kx, const dType Kv,
                                   const dType Pxx_pri_t, const dType Pxv_pri_t, const dType Pvv_pri_t,
                                   dType &Pxx_t, dType &Pxv_t, dType &Pvv_t);
private:
    TRACK_METHOD calcTrackMethod{TRACK_METHOD::TRACK_NONE};
};

#endif // KALMANCALC_H
