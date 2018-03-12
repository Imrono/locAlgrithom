#include "calcLibGeometry.h"
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

bool calcCross2Circle(const dType ax, const dType ay, const dType ad,
                      const dType bx, const dType by, const dType bd,
                      dType &x0_1, dType &y0_1, dType &x0_2, dType &y0_2, bool &isCross) {
    double x1, y1, x2, y2, d1 = ad, d2 = bd;
    bool isReversed = false;
    if (qAbs(ax - bx) < MY_EPS && qAbs(ay - by) < MY_EPS) {
        return false;
    } else if (qAbs(ax - bx) <= qAbs(ay - by)) {
        isReversed = false;
        x1 = ax;
        y1 = ay;
        x2 = bx;
        y2 = by;
    } else {
        isReversed = true;
        x1 = ay;
        y1 = ax;
        x2 = by;
        y2 = bx;
    }

    dType m = 0.5f*(((x1*x1-x2*x2)+(y1*y1-y2*y2)-(d1*d1-d2*d2)) / (y1-y2));
    dType n = -(x1-x2)/(y1-y2);

    dType sqrtItem = 2.f*(n*x1+m)*y1 - y1*y1 - n*n*x1*x1 - 2.f*m*n*x1 - m*m + (1.f+n*n)*d1*d1;

    //qDebug() << m << n << sqrtItem ;

    dType a = -(m*n-n*y1-x1);
    dType b = 1.f+n*n;
    if (sqrtItem > 0.f) {
        isCross = true;

        x0_1 = (a + qSqrt(sqrtItem)) / b;
        y0_1 = m + n*x0_1;

        x0_2 = (a - qSqrt(sqrtItem)) / b;
        y0_2 = m + n*x0_2;
    } else {
        isCross = false;

        x0_1 = a / b;
        y0_1 = m + n*x0_1;

        x0_2 = qSqrt(-sqrtItem) / b;
        y0_2 = n*x0_2;
    }

    if (isReversed) {
        dType tmp = x0_1;
        x0_1 = y0_1;
        y0_1 = tmp;
        tmp = x0_2;
        x0_2 = y0_2;
        y0_2 = tmp;
    }
    return true;
}
