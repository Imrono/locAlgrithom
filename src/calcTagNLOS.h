#ifndef IDENTIFYNLOS_H
#define IDENTIFYNLOS_H
#include "_myheads.h"
#include "dataType.h"

class calcTagNLOS
{
public:
    calcTagNLOS();
    ~calcTagNLOS();

    /*************************************************************/
    POINTS_NLOS predictNlos{POINTS_NLOS::POINTS_NONE};
    bool pointsPredictNlos(dist4Calc &distCurr, int nSensor,
                           const QVector<dist4Calc> &distRefined) const;
    bool isWylieNLOS(int &d_t, dType *d, dType &L_hat) const ;
    bool isMultiPointNLOS(int *d_t, const int *d_t_1, int nSensor) const;

    /*************************************************************/
    void refineWylieNLOS(dType L_hat, int &distCurr) const;
    void refineMultiPointNLOS(/*IN_OUT*/int *d_t, /*IN*/const int *d_t_1, int nSensor) const;

    /*************************************************************/
    POS_PRECISION_NLOS precNlos{POS_PRECISION_NLOS::POS_PRECISION_NONE};
    bool posPrecisionNLOS(dType precision) const;
    bool isResNLOS(dType res) const;
    bool isSumDistNLOS(dType sumDist) const;

private:
    /*************************************************************/
    dType wylieThreshold  {25.f};
    int   wylieN          {4};
    int   wylieOrder      {2};
    dType wylieRatio      {.3f};
    dType *wylie_B        {nullptr};
    dType *wylie_X        {nullptr};
    dType *wylie_A_data   {nullptr};
    dType **wylie_A       {nullptr};
    dType *wylieData      {nullptr};

    dType resThreshold    {10000.f};
    dType sumDistThreshold{50.f};
    /*************************************************************/
    dType multiPointFactor{1.7f};
    /*************************************************************/
    dType multiPointRatio {0.3f};
};

#endif // IDENTIFYNLOS_H
