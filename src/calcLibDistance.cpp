#include "calcLibDistance.h"
#include <QtMath>
#include <QLineF>

dType calcDistanceSquare(const locationCoor &a, const locationCoor &b) {
    dType ans = (a.x - b.x) * (a.x - b.x)
              + (a.y - b.y) * (a.y - b.y)
              + (a.z - b.z) * (a.z - b.z);
    return ans;
}
dType calcDistanceSquare(const QPointF &a, const QPointF &b) {
    dType ans = (a.x() - b.x()) * (a.x() - b.x())
              + (a.y() - b.y()) * (a.y() - b.y());
    return ans;
}

dType calcDistanceMSE(const dType *a, const dType *b, int N) {
    dType ans = 0.f;
    for (int i = 0; i < N; i++) {
        ans += (a[i] - b[i]) * (a[i] - b[i]);
    }
    ans /= N;
    return ans;
}
dType calcDistanceMSE(const dType *dist, const dType *X, const locationCoor *sensor, int N) {
    //dType *d_hat = new dType[N];
    dType d_hat[MAX_SENSOR];
    for (int i = 0; i < N; i++) {
        //qDebug() << "calcDistanceMSE" << X[0] << X[1] << sensor[i].toString()
        //         << qPow(X[0]-sensor[i].x, 2) << qPow(X[1]-sensor[i].y, 2);
        d_hat[i] = qSqrt(qPow(X[0]-sensor[i].x, 2) + qPow(X[1]-sensor[i].y, 2));
    }
    dType ans = calcDistanceMSE(dist, d_hat, N);
    //delete []d_hat;
    return ans;
}

dType calcDistance(const locationCoor &a, const locationCoor &b) {
    dType ans = (a.x - b.x) * (a.x - b.x)
              + (a.y - b.y) * (a.y - b.y)
              + (a.z - b.z) * (a.z - b.z);
    ans = qSqrt(ans);
    return ans;
}
dType calcDistance(const QPointF &a, const QPointF &b) {
    return calcDistance({dType(a.x()), dType(a.y()), 0}, {dType(b.x()), dType(b.y()), 0});
}
dType calcTotalDistance(QVector<QLineF> &lines, int discount) {
    dType ans = 0.0f;
    for(int i = discount; i < lines.count(); i++) {
        ans += calcDistance(lines[i].p1(), lines[i].p2());
    }
    return ans;
}
dType calcTotalAvgDistance(QVector<QLineF> &lines, int discount) {
    return calcTotalDistance(lines, discount)/(lines.count()-dType(discount));
}
dType calcTotalDistanceSquare(QVector<QLineF> &lines, int discount) {
    dType ans = 0.0f;
    for(int i = discount; i < lines.count(); i++) {
        ans += calcDistanceSquare(lines[i].p1(), lines[i].p2());
    }
    return ans;
}
dType calcTotalAvgDistanceSquare(QVector<QLineF> &lines, int discount) {
    return calcTotalDistanceSquare(lines, discount)/(lines.count()-dType(discount));
}

