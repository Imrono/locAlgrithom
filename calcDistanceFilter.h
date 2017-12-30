#ifndef CALCDISTANCEFILTER_H
#define CALCDISTANCEFILTER_H
#include "_myheads.h"

class calcDistanceFilter
{
public:
    calcDistanceFilter();

    void refineWylieNLOS(dType *d_predict, dType *d_meas, int nSensor, dType *out_refined_d);
    void refineMultiPointNLOS(dType *diff_d, dType *d_meas, int n, dType *out_refined_d);
};

#endif // CALCDISTANCEFILTER_H
