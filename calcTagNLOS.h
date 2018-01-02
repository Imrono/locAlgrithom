#ifndef IDENTIFYNLOS_H
#define IDENTIFYNLOS_H
#include "_myheads.h"
#include "datatype.h"

class calcTagNLOS
{
public:
    calcTagNLOS();

    /*************************************************************/
    enum POINTS_NLOS {
        POINTS_NONE = -1,
        WYLIE,
        MULTI_POINT
    };
    POINTS_NLOS predictNlos{POINTS_NONE};
    bool pointsPredictNlos(labelDistance &distCurr, int nSensor,
                           const QVector<labelDistance> &distRefined) const;
    bool isWylieNLOS(dType **d, int nTime, int nSensor) const ;
    bool isMultiPointNLOS(int *d_t, const int *d_t_1, int nSensor) const;

    /*************************************************************/
    void refineWylieNLOS(dType *d_history, dType *d_meas, int nSensor,
                         dType *d_predict, int *idx, int &num) const;
    void refineMultiPointNLOS(dType avgDist_noMax, dType maxDist, dType d_t_1, dType &d_ans) const;

    /*************************************************************/
    enum POS_PRECISION_NLOS {
        POS_PRECISION_NONE = -1,
        RESIDUAL,
        SUM_DIST
    };
    POS_PRECISION_NLOS precNlos{POS_PRECISION_NONE};
    bool posPrecisionNLOS(dType precision) const;
    bool isResNLOS(dType res) const;
    bool isSumDistNLOS(dType sumDist) const;

private:
    /*************************************************************/
    dType wylieThreshold  {0.f};
    int   wylieN          {4};
    dType resThreshold    {10000.f};
    dType sumDistThreshold{50.f};
    /*************************************************************/
    dType multiPointFactor{1.7f};
    /*************************************************************/
    dType multiPointRatio {0.3f};
};

#endif // IDENTIFYNLOS_H
