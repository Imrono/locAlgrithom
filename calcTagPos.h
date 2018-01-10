#ifndef CALCPOS_H
#define CALCPOS_H
#include "_myheads.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"
#include <QVector>
#include "dataType.h"
#include "showStore.h"
#include "calcTagNLOS.h"
#include "calcLibDistance.h"

class calcTagPos
{
public:
    QVector<dist4Calc> distRefined;

    explicit calcTagPos() {}
    explicit calcTagPos(const configData *cfg_q, const distanceData *dist_q) {
        this->cfg_d  = cfg_q;
        this->dist_d = dist_q;
    }
    ~calcTagPos();
    void setConfigData(const configData *cfg_q);
    void setDistanceData(const distanceData *dist_q);
    void setNlosJudge(const calcTagNLOS *calcNlos);

    QVector<locationCoor> calcPosFromDistance(const int dist[], uint32_t count = 4) const;
    static locationCoor calcOnePosFor2Dim(dType dist[], locationCoor loca[]);

    // ALL IN ONE
    CALC_POS_TYPE calcPosType{CALC_POS_TYPE::POS_NONE};
    locationCoor calcOnePosition(const int *dist, dType &MSE);

    locationCoor calcFullCentroid(const int *dist, dType &MSE);
    locationCoor calcSubLS       (const int *dist, dType &MSE);
    locationCoor calcTwoCenter   (const int *dist, dType &MSE);
    locationCoor calcTaylorSeries(const int *dist, dType &MSE);

    static void calcFullCentroid(const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);
    static void calcSubLS       (const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);
    static void calcTwoCenter   (const int *distance, const locationCoor *sensor, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);
    static void calcTaylorSeries(const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType **A_taylor, dType *B_taylor,
                                 dType &out_x, dType &out_y, dType &out_MSE);

    void calcPosVector (storeTagInfo *label);

    locationCoor getLoc(int idx) const {
        return cfg_d->sensor[idx];
    }

private:
    const configData   *cfg_d {nullptr};
    const distanceData *dist_d{nullptr};

    const calcTagNLOS  *calcNlos{nullptr};

    void resetA();
    dType **A_fc{nullptr};
    dType **A_fc_inverse_AT{nullptr};
    dType  *B_fc{nullptr};
    int fc_row{0};    //number of sensor
    int fc_col{0};    //x,y,x or x,y

    dType **A_ls{nullptr};
    dType **A_ls_inverse_AT{nullptr};
    dType  *B_ls{nullptr};
    int ls_row{0};    //number of sensor
    int ls_col{0};    //x,y,x or x,y

    dType *B_taylor{nullptr};
    dType **A_taylor{nullptr};

    dType X[3]{0.f};

    int iterCount{1};
};

#endif // CALCPOS_H
