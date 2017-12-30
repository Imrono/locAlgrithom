#ifndef IDENTIFYNLOS_H
#define IDENTIFYNLOS_H
#include "_myheads.h"

class calcTagNLOS
{
public:
    calcTagNLOS();

    bool isNLOS();

    bool isWylieNLOS(dType **d, int nTime, int nSensor, dType *d_predict);
    bool isMultiPointNLOS(dType *d_t, dType *d_t_1, int n, dType *diff_d);
    bool isResNLOS(dType res);
    bool isSumDistNLOS(dType sumDist);

    bool isWylie{false};
    bool isMultiPoint{false};
    bool isRes{false};
    bool isSumDist{false};

private:
    dType wylieThreshold;
    int   wylieN          {4};
    dType resThreshold    {10000.f};
    dType sumDistThreshold{80.f};
    dType cmpFactor       {1.7f};
    dType smootherRatio   {0.3f};
};

#endif // IDENTIFYNLOS_H
