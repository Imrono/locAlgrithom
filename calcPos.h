#ifndef CALCPOS_H
#define CALCPOS_H
#include <QVector>
#include "datatype.h"
#include "showStore.h"

dType calcDistanceSquare(const locationCoor &a, const locationCoor &b);
dType calcDistanceSquare(const QPoint &a, const QPoint &b);
dType calcDistance(const locationCoor &a, const locationCoor &b);
dType calcDistance(const QPoint &a, const QPoint &b);
dType calcTotalDistance(QVector<QLine> &lines, int discount = 10);
dType calcTotalAvgDistance(QVector<QLine> &lines, int discount = 10);
dType calcTotalDistanceSquare(QVector<QLine> &lines, int discount = 10);
dType calcTotalAvgDistanceSquare(QVector<QLine> &lines, int discount = 10);

class calcPos
{
public:
    QVector<labelDistance> dist;
    QVector<labelDistance> distRefined;

    calcPos();
    calcPos(locationCoor locIn[4]) {
        loc[0] = locIn[0];
        loc[1] = locIn[1];
        loc[2] = locIn[2];
        loc[3] = locIn[3];
    }
    calcPos(calcPos &other) {
        loc[0] = other.loc[0];
        loc[1] = other.loc[1];
        loc[2] = other.loc[2];
        loc[3] = other.loc[3];
    }

    QVector<locationCoor> calcPosFromDistance(const uint32_t dist[], uint32_t count = 4) const;
    distance_3 calcMin3Loca(uint32_t dist[], uint32_t count = 4) const;
    static locationCoor calcOnePos(dType dist[], locationCoor loca[]);
    static locationCoor calcOnePos(distance_3 info) {
        return calcPos::calcOnePos(info.dist, info.loca);
    }
    void calcPosVector (labelInfo *label);
    void calcPotimizedPos(labelInfo *label);
    void calcPosVectorWylie (labelInfo *label);
    void calcPotimizedPosWylie(labelInfo *label);

    locationCoor getLoc(int idx) const {
        return loc[idx];
    }

private:
    locationCoor loc[4];
};

#endif // CALCPOS_H
