#ifndef CALCPOS_H
#define CALCPOS_H
#include <QVector>
#include "datatype.h"
#include "showStore.h"

double distanceSquare(const locationCoor &a, const locationCoor &b);
double distanceSquare(const QPoint &a, const QPoint &b);
double calcDistance(const locationCoor &a, const locationCoor &b);
double calcDistance(const QPoint &a, const QPoint &b);
double calcTotalDistance(QVector<QLine> &lines, int discount = 10);
double calcTotalAvgDistance(QVector<QLine> &lines, int discount = 10);

class calcPos
{
public:
    QVector<labelDistance> dist;

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

    QVector<locationCoor> calcPosFromDistance(uint32_t dist[], uint32_t count = 4);
    distance_3 calcMin3Loca(uint32_t dist[], uint32_t count = 4);
    static locationCoor calcOnePos(double dist[], locationCoor loca[]);
    static locationCoor calcOnePos(distance_3 info) {
        return calcPos::calcOnePos(info.dist, info.loca);
    }
    void calcPosVector (labelInfo *label);
    void calcPotimizedPos(labelInfo *label);

    locationCoor getLoc(int idx) {
        return loc[idx];
    }

private:
    locationCoor loc[4];
};

#endif // CALCPOS_H
