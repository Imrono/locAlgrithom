#ifndef DATATYPE_H
#define DATATYPE_H
#include <QPoint>
#include <QDateTime>

#ifndef dType
#define dType double
#endif

struct labelDistance {
    uint32_t distance[4];
    uint32_t status[4];
    QDateTime time[4];

    static labelDistance diffDist(const labelDistance &a, const labelDistance &b) {
        labelDistance ans;
        ans.distance[0] = qAbs(int(a.distance[0]) - int(b.distance[0]));
        ans.distance[1] = qAbs(int(a.distance[1]) - int(b.distance[1]));
        ans.distance[2] = qAbs(int(a.distance[2]) - int(b.distance[2]));
        ans.distance[3] = qAbs(int(a.distance[3]) - int(b.distance[3]));
        return ans;
    }

    QString toStringDist() {
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
