#ifndef DATATYPE_H
#define DATATYPE_H
#include "_myheads.h"
#include <QPoint>
#include <QDateTime>

struct oneLogData_1 {
    QDateTime time;
    int distance;
    int sensorIdx;
    int status;

    QString toString() {
        return QString("[%0] sensorIdx:%2, distance:%3, status:%4")
                .arg(time.toString("yyyy/MM/dd hh:mm:ss:zzz"))
                .arg(sensorIdx, 3, 10, QChar('0'))
                .arg(distance,  6, 10, QChar('0'))
                .arg(status,    2, 10, QChar('0'));
    }
};
struct oneLogData_2 {
    QDateTime time;
    QVector<int> distance;
    QString toString() {
        QString ans = QString("[%0]").arg(time.toString("yyyy/MM/dd hh:mm:ss:zzz"));
        for (int i = 0; i < distance.count(); i++) {
            ans += QString("{%0,%1}").arg(i).arg(distance[i], 6, 10, QChar('0'));
        }
        return ans;
    }
};

struct oneTag_1 {
    oneTag_1(int idx) : tagIdx{idx} {}
    oneTag_1() {}

    int tagIdx;
    QVector<oneLogData_1> distData;

    QString toString() {
        return QString("tagIdx:%0").arg(tagIdx, 5, QChar('0'));
    }
};
struct oneTag_2 {
    oneTag_2(int idx) : tagIdx{idx} {}
    oneTag_2() {}

    int tagIdx;
    QVector<oneLogData_2> distData;
};

struct labelDistance {
    int distance[6];
    uint32_t status[6];
    QDateTime time[6];

    static labelDistance diffDist(const labelDistance &a, const labelDistance &b) {
        labelDistance ans;
        ans.distance[0] = qAbs(a.distance[0] - b.distance[0]);
        ans.distance[1] = qAbs(a.distance[1] - b.distance[1]);
        ans.distance[2] = qAbs(a.distance[2] - b.distance[2]);
        ans.distance[3] = qAbs(a.distance[3] - b.distance[3]);
        return ans;
    }

    QString toStringDist() const {
        return QString("(%0,%1,%2,%3)")
                .arg(distance[0],4,10,QChar('0'))
                .arg(distance[1],4,10,QChar('0'))
                .arg(distance[2],4,10,QChar('0'))
                .arg(distance[3],4,10,QChar('0'));
    }
};

struct locationCoor
 {
    dType x;
    dType y;
    dType z;

    locationCoor()
    {}
    locationCoor(dType x, dType y, dType z) : x{x}, y{y}, z{z}
    {}
    locationCoor(const locationCoor &obj): x(obj.x), y(obj.y), z(obj.z)
    {}
    locationCoor(const QPoint &obj): x(obj.x()), y(obj.y()), z(0.0f)
    {}
    locationCoor operator=(const locationCoor &t) {
        x = t.x;
        y = t.y;
        z = t.z;
        return *this;
    }
    locationCoor operator+(const locationCoor &t) const {
        locationCoor ans(*this);
        ans.x += t.x;
        ans.y += t.y;
        ans.z += t.z;
        return ans;
    }
    locationCoor operator-(const locationCoor &t) const {
        locationCoor ans(*this);
        ans.x -= t.x;
        ans.y -= t.y;
        ans.z -= t.z;
        return ans;
    }
    locationCoor operator*(const dType &t) const {
        locationCoor ans(*this);
        ans.x *= t;
        ans.y *= t;
        ans.z *= t;
        return ans;
    }
    locationCoor operator/(const dType &t) const {
        locationCoor ans(*this);
        ans.x /= t;
        ans.y /= t;
        ans.z /= t;
        return ans;
    }
    QString toString() const {
        return QString("(%0,%1,%2)").arg(x).arg(y).arg(z);
    }
    QPoint toQPoint() const {
        return {int(x), int(y)};
    }
    QPointF toQPointF() const {
        return {x, y};
    }
};

struct coorLine {
    locationCoor A;
    locationCoor B;
};

struct distance_3 {
    dType dist[3];
    locationCoor loca[3];
    QString toString() const {
        QString ans =  QString("%0@%1,%2@%3,%4@%5")
                .arg(loca[0].toString(), 14).arg(dist[0], 5, 'g', 3)
                .arg(loca[1].toString(), 14).arg(dist[1], 5, 'g', 3)
                .arg(loca[2].toString(), 14).arg(dist[2], 5, 'g', 3);
        return ans;
    }
};

#endif // DATATYPE_H
