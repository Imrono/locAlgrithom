#ifndef CALCDISTANCE_H
#define CALCDISTANCE_H
#include "_myheads.h"
#include "datatype.h"

// distance calculate
dType calcDistanceSquare(const locationCoor &a, const locationCoor &b);
dType calcDistanceSquare(const QPointF &a, const QPointF &b);

dType calcDistanceMSE(const dType *a, const dType *b, int N);
dType calcDistanceMSE(const dType *dist, const dType *X, const locationCoor *sensor, int N);

dType calcDistance(const locationCoor &a, const locationCoor &b);
dType calcDistance(const QPointF &a, const QPointF &b);

dType calcTotalDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistance(QVector<QLineF> &lines, int discount = 10);
dType calcTotalDistanceSquare(QVector<QLineF> &lines, int discount = 10);
dType calcTotalAvgDistanceSquare(QVector<QLineF> &lines, int discount = 10);

bool calcCross2Circle(const dType ax, const dType ay, const dType ad,
                      const dType bx, const dType by, const dType bd,
                      dType &x0_1, dType &y0_1, dType &x0_2, dType &y0_2, bool &isCross);

#endif // CALCDISTANCE_H
