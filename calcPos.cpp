#include "calcPos.h"
#include <QtMath>
#include <QDebug>

double calcDistanceSquare(const locationCoor &a, const locationCoor &b) {
    double ans = (a.x - b.x) * (a.x - b.x)
               + (a.y - b.y) * (a.y - b.y)
               + (a.z - b.z) * (a.z - b.z);
    return ans;
}
double calcDistanceSquare(const QPoint &a, const QPoint &b) {
    double ans = (a.x() - b.x()) * (a.x() - b.x())
               + (a.y() - b.y()) * (a.y() - b.y());
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
double calcTotalDistance(QVector<QLine> &lines, int discount) {
    double ans = 0.0f;
    for(int i = discount; i < lines.count(); i++) {
        ans += calcDistance(lines[i].p1(), lines[i].p2());
    }
    return ans;
}
double calcTotalAvgDistance(QVector<QLine> &lines, int discount) {
    return calcTotalDistance(lines, discount)/(lines.count()-double(discount));
}
double calcTotalDistanceSquare(QVector<QLine> &lines, int discount) {
    double ans = 0.0f;
    for(int i = discount; i < lines.count(); i++) {
        ans += calcDistanceSquare(lines[i].p1(), lines[i].p2());
    }
    return ans;
}
double calcTotalAvgDistanceSquare(QVector<QLine> &lines, int discount) {
    return calcTotalDistanceSquare(lines, discount)/(lines.count()-double(discount));
}

calcPos::calcPos()
{

}

distance_3 calcPos::calcMin3Loca(uint32_t dist[], uint32_t count) const {
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

    //qDebug() << QString("a11(%0)*x + a12(%1)*y = b1(%2)").arg(a11, 6).arg(a12, 6).arg(b1, 6);
    //qDebug() << QString("a21(%0)*x + a22(%1)*y = b2(%2)").arg(a21, 6).arg(a22, 6).arg(b2, 6);

    double y_pos = (b1*a21-b2*a11) / (a12*a21-a22*a11);
    double x_pos = (b1*a22-b2*a12) / (a11*a22-a21*a12);
    double z_pos = 0.0f;
    //qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return locationCoor{x_pos, y_pos, z_pos};
}

QVector<locationCoor> calcPos::calcPosFromDistance(const uint32_t dist[], uint32_t count) const {
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
                //qDebug() << info.toString() << pos.toString();
            }
        }
    }
    //qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return vectorAns;
}

void calcPos::calcPosVector (labelInfo *label) {
    if (nullptr == label)
        return;
    label->RawPoints.clear();
    for (int i = 0; i < dist.count(); i++) {
        label->RawPoints.append(calcPosFromDistance(dist[i].distance, 4));

        if (i >= 1) {
            double factor = 1.7;
            double totalDistance = 0.0f;
            double maxDist = 0.0f;
            double avgDist_noMax = 0.0f;
            uint32_t distance[4] = {0};
            for (int j = 0; j < 4; j++) {
                distance[j] = dist[i].distance[j];
            }
            for(int i1 = 0; i1 < 4; i1++) {
                for(int i2 = i1; i2 < 4; i2++) {
                    totalDistance += calcDistance(label->RawPoints[label->RawPoints.count()-1][i1],
                            label->RawPoints[label->RawPoints.count()-1][i2]);
                }
            }
            int maxIdx = -1;
            for (int j = 0; j < 4; j++) {
                //double diffDist = qAbs(double(dist[i].distance[j]) - double(dist[i-1].distance[j]));
                double diffDist = qAbs(double(dist[i].distance[j]) - double(distRefined[i-1].distance[j]));
                if (maxDist <= diffDist) {
                    maxDist = diffDist;
                    maxIdx = j;
                }
                avgDist_noMax += diffDist;
            }
            avgDist_noMax = (avgDist_noMax - maxDist) / 3.0f;
            if (maxDist > avgDist_noMax * factor
             && totalDistance > 80.f) {
                //distance[maxIdx] = dist[i-1].distance[maxIdx] + avgDist_noMax;
                distance[maxIdx] = distRefined[i-1].distance[maxIdx]
                        + uint32_t((double(dist[i].distance[maxIdx]) - double(distRefined[i-1].distance[maxIdx])) * 0.3);
            }
            label->RefinedPoints.append(calcPosFromDistance(distance, 4));

            labelDistance tmpDist;
            tmpDist.distance[0] = distance[0];
            tmpDist.distance[1] = distance[1];
            tmpDist.distance[2] = distance[2];
            tmpDist.distance[3] = distance[3];
            distRefined.append(tmpDist);
            //qDebug() << i << dist[i].distance[0] << dist[i].distance[1] << dist[i].distance[2] << dist[i].distance[3]
            //         << avgDist_noMax << maxDist << (maxDist > avgDist_noMax * factor) << (totalDistance > 80.f);
        } else {
            label->RefinedPoints.append(calcPosFromDistance(dist[i].distance, 4));
            distRefined.append(dist[i]);
        }
    }
}

void calcPos::calcPotimizedPos(labelInfo *label) {
    locationCoor optimizedPos = {0, 0, 0};
    locationCoor center = {0.0f, 0.0f, 0.0f};
    locationCoor p_1 = {0, 0, 0};
    //const QVector<QVector<locationCoor>> &calcPoints = label->RawPoints;
    const QVector<QVector<locationCoor>> &calcPoints = label->RefinedPoints;
    int numData = calcPoints.count();

    /*
    double maxDistance = 0.0f;
    double tmpDistance = 0.0f;
    int idx = -1;

    for (int loop = 0; loop < 0; loop++) {
        for (int i = 0; i < label->RawPoints.count(); i++) {
            center = center + label->RawPoints[i] / label->RawPoints.count();
        }
        for (int i = 0; i < label->RawPoints.count(); i++) {
            tmpDistance = calcDistance(center, label->RawPoints[i]);
            if (maxDistance < tmpDistance) {
                maxDistance = tmpDistance;
                idx = i;
            }
        }
        Positions.removeAt(idx);
        center = {0.0f, 0.0f, 0.0f};
        maxDistance = 0.0f;
    }
    */

    for (int i = 0; i < numData; i++) {
        //distance_3 min_3 = calcMin3Loca(dist[i].distance, 4);
        //locationCoor min_Coor = calcPos::calcOnePos(min_3);
        //qDebug() << "min_3 location Coordiator:" << min_3.toString() << min_Coor.toString();
        //optimizedPos = min_Coor;
        //label->Ans.append(optimizedPos);
        //label->Reliability.append(1.0f);

        int rawPoints = calcPoints[i].count();
        for (int j = 0; j < rawPoints; j++) {
            center = center + calcPoints[i][j] / double(calcPoints[i].count());
        }
        optimizedPos = center;
        double totalDistance = 0.0f;
        for(int i1 = 0; i1 < rawPoints; i1++) {
            for(int i2 = 0; i2 < rawPoints; i2++) {
                totalDistance += calcDistance(calcPoints[i][i1], calcPoints[i][i2]) / 2.0f;
            }
        }
        label->Ans.append(optimizedPos);
        label->Reliability.append(totalDistance);

        if (i > 0)
            label->AnsLines.append(QLine(p_1.toQPoint(), optimizedPos.toQPoint()));
        p_1 = optimizedPos;
        center = {0, 0, 0};
    }
}
