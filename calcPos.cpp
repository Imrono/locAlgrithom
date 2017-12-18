#include "calcPos.h"
#include <QtMath>
#include <QDebug>

double distanceSquare(const locationCoor &a, const locationCoor &b) {
    double ans = (a.x - b.x) * (a.x - b.x)
               + (a.y - b.y) * (a.y - b.y)
               + (a.z - b.z) * (a.z - b.z);
    return ans;
}
double calcDistance(const locationCoor &a, const locationCoor &b) {
    double ans = (a.x - b.x) * (a.x - b.x)
               + (a.y - b.y) * (a.y - b.y)
               + (a.z - b.z) * (a.z - b.z);
    ans = qSqrt(ans);
    return ans;
}
double calcDistance(const QPoint &a, const QPoint &b) {
    return calcDistance({double(a.x()), double(a.y()), 0}, {double(b.x()), double(b.y()), 0});
}

calcPos::calcPos()
{

}

distance_3 calcPos::calcMin3Loca(uint32_t dist[], uint32_t count) {
    distance_3 min_3;

    uint32_t num = count;
    uint32_t tmp = 0;
    uint32_t idx;
    QVector<uint32_t> removeIdx;
    for(; num > 3; num--) {
        for(uint32_t i = 0; i < count; i++) {
            if (dist[i] > tmp) {
                idx = i;
                tmp = dist[i];
            }
        }
        tmp = 0;
        removeIdx.append(idx);
    }

    bool flag = false;
    int k = 0;
    for (uint32_t i = 0; i < count; i++) {
        for (int j = 0; j < removeIdx.count(); j++) {
            if (i == removeIdx[j]) {
                flag = true;
                break;
            }
        }
        if (false == flag) {
            min_3.dist[k] = double(dist[i])/10.0f;
            min_3.loca[k].x = loc[i].x;
            min_3.loca[k].y = loc[i].y;
            min_3.loca[k].z = loc[i].z;
            k++;
        }
        flag = false;
    }

    return min_3;
}

locationCoor calcPos::calcOnePos(double dist[], locationCoor loca[]) {
    double b1 = dist[0]*dist[0] - dist[1]*dist[1]
            + (loca[1].x+loca[0].x)*(loca[1].x-loca[0].x)
            + (loca[1].y+loca[0].y)*(loca[1].y-loca[0].y);
    double b2 = dist[0]*dist[0] - dist[2]*dist[2]
            + (loca[2].x+loca[0].x)*(loca[2].x-loca[0].x)
            + (loca[2].y+loca[0].y)*(loca[2].y-loca[0].y);

    double a11 = 2*(loca[1].x-loca[0].x);
    double a12 = 2*(loca[1].y-loca[0].y);

    double a21 = 2*(loca[2].x-loca[0].x);
    double a22 = 2*(loca[2].y-loca[0].y);

    qDebug() << QString("a11(%0)*x + a12(%1)*y = b1(%2)").arg(a11, 6).arg(a12, 6).arg(b1, 6);
    qDebug() << QString("a21(%0)*x + a22(%1)*y = b2(%2)").arg(a21, 6).arg(a22, 6).arg(b2, 6);

    double y_pos = (b1*a21-b2*a11) / (a12*a21-a22*a11);
    double x_pos = (b1*a22-b2*a12) / (a11*a22-a21*a12);
    double z_pos = 0.0f;
    qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return locationCoor{x_pos, y_pos, z_pos};
}
locationCoor calcPos::calcOnePos(distance_3 info) {
    return calcPos::calcOnePos(info.dist, info.loca);
}

QVector<locationCoor> calcPos::calcPosFromDistance(uint32_t dist[], uint32_t count) {
    //qDebug() << "dist in:" << dist[0] << dist[1] << dist[2] << dist[3];
    //distance_3 min_3 = calcMin3Loca(dist, count);
    //locationCoor min_Coor = calcPos::calcOnePos(min_3);
    //qDebug() << "min_3 location Coordiator:" << min_3.toString() << min_Coor.toString();

    QVector<locationCoor> vectorAns;
    distance_3 info;
    for (uint32_t i = 0; i < count - 2; i++) {
        for (uint32_t j = i+1; j < count - 1; j++) {
            for (uint32_t k = j+1; k < count; k++) {
                info.dist[0] = double(dist[i])/10.0f;
                info.dist[1] = double(dist[j])/10.0f;
                info.dist[2] = double(dist[k])/10.0f;
                info.loca[0] = loc[i];
                info.loca[1] = loc[j];
                info.loca[2] = loc[k];
                locationCoor pos = calcPos::calcOnePos(info);
                vectorAns.append(pos);
                qDebug() << info.toString() << pos.toString();
            }
        }
    }
    //qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return vectorAns;
}
