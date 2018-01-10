#ifndef MYHEADS_H
#define MYHEADS_H

#include <QVector>
#include <QDebug>

#ifndef dType
#define dType double
#endif

#ifndef MY_STR
#define MY_STR(x)   QString::fromLocal8Bit(x)
#endif

#ifndef MY_EPS
#define MY_EPS  0.0001f
#endif

#define MEASUR_STR "measure"
#define TRACKx_STR "track"

enum CALC_POS_TYPE {
    POS_NONE = -1,
    FullCentroid = 0,
    SubLS = 1,
    TwoCenter = 2,
    Taylor = 3,
    NUM_CALC_POS
};

#define METHOD_FULL_CENTROID_STR "{method:FullCentroid}"
#define METHOD_SUB_LS_STR        "{method:SubLS}"
#define METHOD_TWO_CENTER_STR    "{method:TwoCenter}"
#define METHOD_TAYLOR_SERIES_STR "{method:TaylorSeries}"
extern QString CALC_POS2STR[CALC_POS_TYPE::NUM_CALC_POS];

enum POINTS_NLOS {
    POINTS_NONE = -1,
    WYLIE,
    MULTI_POINT
};
enum POS_PRECISION_NLOS {
    POS_PRECISION_NONE = -1,
    RESIDUAL,
    SUM_DIST
};

enum TRACK_METHOD {
    TRACK_NONE = -1,
    TRACK_KALMAN,
    TRACK_KALMAN_LITE,
    NUM_TRACK_METHOD
};

#define METHOD_KALMAN_STR      "{method:KalmanTrack}"
#define METHOD_KALMAN_LITE_STR "{method:LiteKalmanTrack}"
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
    HAS_TRACK_DATA
};

#endif // MYHEADS_H
