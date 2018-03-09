#ifndef MYHEADS_H
#define MYHEADS_H

#include <QVector>
#include <QDebug>

#define MAX_SENSOR 10

#ifndef dType
#define dType float
#endif

#ifndef MY_STR
#define MY_STR(x)   QString::fromLocal8Bit(x)
#endif

#ifndef MY_EPS
#define MY_EPS  0.000001f
#endif

#define MEASUR_STR "measure"
#define TRACKx_STR "track"

enum CALC_POS_TYPE {
    POS_NONE = -1,
    FullCentroid = 0,
    SubLS = 1,
    TwoCenter = 2,
    Taylor = 3,
    WeightedTaylor,
    POS_KalmanLoose,
    POS_KalmanMedium,
    POS_KalmanTight,
    POS_KalmanUltraTight,
    LMedS,
    Bilateration,
    ARM_calcPos,
    NUM_CALC_POS
};

#define METHOD_FULL_CENTROID_STR   "{method:FullCentroid}"
#define METHOD_SUB_LS_STR          "{method:SubLS}"
#define METHOD_TWO_CENTER_STR      "{method:TwoCenter}"
#define METHOD_TAYLOR_SERIES_STR   "{method:TaylorSeries}"
#define METHOD_WEIGHTED_TAYLOR_STR "{method:WeightedTaylor}"
#define METHOD_KALMAN_TAYLOR_STR   "{method:KalmanTaylor}"
#define METHOD_LMEDS_STR           "{method:LMedS}"
extern QString CALC_POS2STR[CALC_POS_TYPE::NUM_CALC_POS];

enum POINTS_NLOS {
    POINTS_NONE = -1,
    WYLIE,
    MULTI_POINT,
    NUM_POINTS_NLOS
};
enum POS_PRECISION_NLOS {
    POS_PRECISION_NONE = -1,
    RESIDUAL,
    SUM_DIST,
    NUM_POS_PRECISION_NLOS
};

enum TRACK_METHOD {
    TRACK_NONE = -1,
    TRACK_KALMAN,
    TRACK_KALMAN_LITE,
    TRACK_KALMAN_INFO,
    NUM_TRACK_METHOD
};

#define METHOD_KALMAN_STR      "{method:KalmanTrack}"
#define METHOD_KALMAN_LITE_STR "{method:LiteKalmanTrack}"
#define METHOD_KALMAN_INFO_STR "{method:InfoKalmanTrack}"
extern QString TRACK_METHOD2STR[TRACK_METHOD::NUM_TRACK_METHOD];

enum SHOW_SHAPE {
    radius,
    triangle,
    square,
    pentagram,
    hexagram,
    pentagon,
    hexagon,
};

enum USR_STATUS {
    HAS_NONE_DATA = -1,
    HAS_DISTANCE_DATA,
    HAS_MEASURE_DATA,
    HAS_TRACK_DATA,
    NUM_USR_STATUS
};

#endif // MYHEADS_H
