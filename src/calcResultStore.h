#ifndef CALCRESULTSTORE_H
#define CALCRESULTSTORE_H
#include "_myheads.h"
#include "calcTagTrack.h"
#include "calcTagPos.h"

struct oneMeasureInput {
    int                         distance{-1};
    uint32_t                    tick{0};
};
struct tagCalcInput {
    oneMeasureInput             measInput[3];
};

struct tagOnePosResult {
    CALC_POS_TYPE               methodType;
    // 坐标计算结果
    QDateTime                   time;
    locationCoor                locCoor;
    // 计算中间结果
    QVector<bool>               usedSensor;
    QVector<QPointF>            iterPoints;
    QVector<dType>              weight;
};
struct tagPosResult {
    QVector<tagOnePosResult>    results;

    // 数据统计结果
    dType                       sigma;
};

struct tagOneTrackResult {
    CALC_TRACK_TYPE             methodType;
    // 坐标计算结果
    QDateTime                   time;
    locationCoor                locCoor;
};
struct tagTrackResult {
    QVector<tagOneTrackResult>  results;

    // 数据统计结果
    dType                       sigma;
};

struct oneTagResult {
    QVector<tagCalcInput *>     calcInput;

    QVector<tagPosResult *>     posResult;
    QVector<tagTrackResult *>   trackResult;

    trackParams                 trackParam;
};

class calcResultStore
{
public:
    calcResultStore();

    QMap<int, oneTagResult *>   tagsResult;
};

#endif // CALCRESULTSTORE_H
