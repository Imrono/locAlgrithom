﻿#include "_myheads.h"

QString CALC_POS2STR[CALC_POS_TYPE::NUM_CALC_POS] =
{
    METHOD_POS_NONE_STR,
    METHOD_MP_POS_IN,
    METHOD_FULL_CENTROID_STR,
    METHOD_SUB_LS_STR,
    METHOD_TWO_CENTER_STR,
    METHOD_TAYLOR_SERIES_STR,
    METHOD_WEIGHTED_TAYLOR_STR,
    METHOD_KALMAN_COUPLED_STR,
    METHOD_KALMAN_COUPLED_STR,
    METHOD_KALMAN_COUPLED_STR,
    METHOD_KALMAN_COUPLED_STR,
    METHOD_KALMAN_COUPLED_STR,
    METHOD_LMEDS_STR,
    METHOD_BILATERATION_STR,
    METHOD_ARM_STR
};

QString TRACK_METHOD2STR[TRACK_METHOD::NUM_TRACK_METHOD] =
{
    METHOD_TRACK_NONE_STR,
    METHOD_KALMAN_STR,
    METHOD_KALMAN_LITE_STR,
    METHOD_KALMAN_INFO_STR,
    METHOD_LOW_PASS_STR
};
