#ifndef CALCPOS_H
#define CALCPOS_H
#include "_myheads.h"
#include "dataSensorIni.h"
#include <QVector>
#include "datatype.h"
#include "showStore.h"

// distance calculate
dType calcDistanceSquare(const locationCoor &a, const locationCoor &b);
dType calcDistanceSquare(const QPointF &a, const QPointF &b);
dType calcDistance(const locationCoor &a, const locationCoor &b);
dType calcDistance(const QPointF &a, const QPointF &b);
dType calcTotalDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalDistanceSquare(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistanceSquare(QVector<QLineF> &lines, int discount = 10);

class calcPos
{
public:
    QVector<labelDistance> dist;
    QVector<labelDistance> distRefined;

    explicit calcPos(const configData *d) {
        this->d = d;
    }

    QVector<locationCoor> calcPosFromDistance(const int dist[], uint32_t count = 4) const;
    distance_3 calcMin3Loca(int dist[], uint32_t count = 4) const;
    static locationCoor calcOnePos(dType dist[], locationCoor loca[]);
    static locationCoor calcOnePos(distance_3 info) {
        return calcPos::calcOnePos(info.dist, info.loca);
    }
    void calcPosVector (labelInfo *label);
    void calcPotimizedPos(labelInfo *label);
    void calcPosVectorWylie (labelInfo *label);
    void calcPotimizedPosWylie(labelInfo *label);

    locationCoor getLoc(int idx) const {
        return d->sensor[idx];
    }

private:
    const configData *d{nullptr};
};

#endif // CALCPOS_H
