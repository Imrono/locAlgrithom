#ifndef MYHEADS_H
#define MYHEADS_H

#include <QVector>
#include <QDebug>

#define MAX_SENSOR 10

#define MY_INT_MAX = ((int)(~(unsigned int)0 >> 1));

#ifndef dType
#define dType float
#endif

#ifndef MY_STR
#define MY_STR(x)   QString::fromLocal8Bit(x)
#endif

#ifndef MY_EPS
#define MY_EPS  0.000001f
#endif

#define MP_POS_STR "mpPos"
#define MEASUR_STR "measure"
#define TRACKx_STR "track"

#define TEST_TAG_ID -2
#define UN_INIT_SHOW_TAGID -1
#define UN_INIT_LM_TAGID -1

enum KALMAN_COUPLED_TYPE{
    NONE_COUPLED   = 0x00,
    TRAIL_COUPLED  = 0x01,
    GAUSS_COUPLED  = 0x02,
    WEIGHT_COUPLED = 0x04,
    SMOOTH_COUPLED = 0x08
};

enum CALC_POS_TYPE {
    POS_NONE = 0,
    Mp_Pos_In,
    FullCentroid,
    SubLS,
    TwoCenter,
    Taylor,
    WeightedTaylor,
    POS_KalmanCoupled,
    POS_KalmanTrail,
    POS_KalmanGauss,
    POS_KalmanWeight,
    POS_KalmanSmooth,
    LMedS,
    Bilateration,
    ARM_calcPos,
    NUM_CALC_POS
};

#define METHOD_POS_NONE_STR        "{method:PosNone}"
#define METHOD_MP_POS_IN           "{method:mpPosIn}"
#define METHOD_FULL_CENTROID_STR   "{method:FullCentroid}"
#define METHOD_SUB_LS_STR          "{method:SubLS}"
#define METHOD_TWO_CENTER_STR      "{method:TwoCenter}"
#define METHOD_TAYLOR_SERIES_STR   "{method:TaylorSeries}"
#define METHOD_WEIGHTED_TAYLOR_STR "{method:WeightedTaylor}"
#define METHOD_KALMAN_COUPLED_STR  "{method:KalmanCoupled}"
#define METHOD_LMEDS_STR           "{method:LMedS}"
#define METHOD_BILATERATION_STR    "{method:Bilateration}"
#define METHOD_ARM_STR             "{method:ARM}"

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
    TRACK_NONE = 0,
    TRACK_KALMAN,
    TRACK_KALMAN_LITE,
    TRACK_KALMAN_INFO,
    TRACK_LOW_PASS,
    NUM_TRACK_METHOD
};

#define METHOD_TRACK_NONE_STR  "{method:TrackNone}"
#define METHOD_KALMAN_STR      "{method:KalmanTrack}"
#define METHOD_KALMAN_LITE_STR "{method:LiteKalmanTrack}"
#define METHOD_KALMAN_INFO_STR "{method:InfoKalmanTrack}"
#define METHOD_LOW_PASS_STR    "{method:LowPassTrack}"
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

#include "_calcParam.h"

#endif // MYHEADS_H
