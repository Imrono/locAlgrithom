#ifndef CALCPOS_H
#define CALCPOS_H
#include "_myheads.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"
#include <QVector>
#include "datatype.h"
#include "showStore.h"

// distance calculate
dType calcDistanceSquare(const locationCoor &a, const locationCoor &b);
dType calcDistanceSquare(const QPointF &a, const QPointF &b);
dType calcDistance(const locationCoor &a, const locationCoor &b);
dType calcDistance(const QPointF &a, const QPointF &b);
dType calcTotalDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalDistanceSquare(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistanceSquare(QVector<QLineF> &lines, int discount = 10);

class calcTagPos
{
public:
    QVector<labelDistance> distRefined;

    enum CALC_POS_TYPE {
        FullCentroid = 0,
        SubLS = 1,
        TwoCenter = 2
    };

    explicit calcTagPos() {}
    explicit calcTagPos(const configData *cfg_q, const distanceData *dist_q) {
        this->cfg_d  = cfg_q;
        this->dist_d = dist_q;
    }
    ~calcTagPos();
    void setConfigData(const configData *cfg_q);
    void setDistanceData(const distanceData *dist_q);

    QVector<locationCoor> calcPosFromDistance(const int dist[], uint32_t count = 4) const;
    distance_3 calcMin3Loca(int dist[], uint32_t count = 4) const;
    static locationCoor calcOnePos(dType dist[], locationCoor loca[]);
    static locationCoor calcOnePos(distance_3 info) {
        return calcTagPos::calcOnePos(info.dist, info.loca);
    }

    //static locationCoor calcOnePosition(QVector<locationCoor> sensor, QVector<dType> dist);
    CALC_POS_TYPE calcPosType;

    locationCoor calcOnePosition(int idx, dType &MSE);

    locationCoor calcFullCentroid(const int *dist, dType &MSE);
    locationCoor calcSubLS       (const int *dist, dType &MSE);
    locationCoor calcTwoCenter   (const int *dist, dType &MSE);

    static void calcFullCentroid(const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);
    static void calcSubLS       (const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);
    static void calcTwoCenter   (const int *distance, const locationCoor *sensor,
                                 dType **A, dType **coA, dType *B, int N,
                                 dType &out_x, dType &out_y, dType &out_MSE);

    // 只有距离突变时，平滑变化最大的距离
    void calcPosVector_1 (labelInfo *label);
    // 距离突变且计算位置可信度不高时，平滑变化最大的距离
    void calcPosVector_2 (labelInfo *label);
    void calcPotimizedPos(labelInfo *label);
    void calcPosVectorWylie (labelInfo *label);
    void calcPotimizedPosWylie(labelInfo *label);
    void calcPosVectorKang (labelInfo *label);
    void calcPosVectorFullCentroid (labelInfo *label);

    locationCoor getLoc(int idx) const {
        return cfg_d->sensor[idx];
    }

private:
    const configData   *cfg_d {nullptr};
    const distanceData *dist_d{nullptr};

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

    dType X[3]{0.f};
};

#endif // CALCPOS_H
