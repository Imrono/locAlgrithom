#include "calcPos.h"
#include <QtMath>
#include <QDebug>
#include "matrixop.h"

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

/***********************************************************************/
distance_3 calcPos::calcMin3Loca(int dist[], uint32_t count) const {
    distance_3 min_3;

    uint32_t num = count;
    int tmp = 0;
    int idx;
    QVector<int> removeIdx;
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
            min_3.dist[k] = dType(dist[i])/10.0f;
            min_3.loca[k].x = d->sensor[i].x;
            min_3.loca[k].y = d->sensor[i].y;
            min_3.loca[k].z = d->sensor[i].z;
            k++;
        }
        flag = false;
    }

    return min_3;
}

locationCoor calcPos::calcOnePos(dType dist[], locationCoor loca[]) {
    dType b1 = dist[0]*dist[0] - dist[1]*dist[1]
            + (loca[1].x+loca[0].x)*(loca[1].x-loca[0].x)
            + (loca[1].y+loca[0].y)*(loca[1].y-loca[0].y);
    dType b2 = dist[0]*dist[0] - dist[2]*dist[2]
            + (loca[2].x+loca[0].x)*(loca[2].x-loca[0].x)
            + (loca[2].y+loca[0].y)*(loca[2].y-loca[0].y);

    dType a11 = 2*(loca[1].x-loca[0].x);
    dType a12 = 2*(loca[1].y-loca[0].y);

    dType a21 = 2*(loca[2].x-loca[0].x);
    dType a22 = 2*(loca[2].y-loca[0].y);

    //qDebug() << QString("a11(%0)*x + a12(%1)*y = b1(%2)").arg(a11, 6).arg(a12, 6).arg(b1, 6);
    //qDebug() << QString("a21(%0)*x + a22(%1)*y = b2(%2)").arg(a21, 6).arg(a22, 6).arg(b2, 6);

    dType y_pos = (b1*a21-b2*a11) / (a12*a21-a22*a11);
    dType x_pos = (b1*a22-b2*a12) / (a11*a22-a21*a12);
    dType z_pos = 0.0f;
    //qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return locationCoor{x_pos, y_pos, z_pos};
}

QVector<locationCoor> calcPos::calcPosFromDistance(const int dist[], uint32_t count) const {
    QVector<locationCoor> vectorAns;
    distance_3 info;
    for (uint32_t i = 0; i < count - 2; i++) {
        for (uint32_t j = i+1; j < count - 1; j++) {
            for (uint32_t k = j+1; k < count; k++) {
                info.dist[0] = dType(dist[i]);
                info.dist[1] = dType(dist[j]);
                info.dist[2] = dType(dist[k]);
                info.loca[0] = d->sensor[i];
                info.loca[1] = d->sensor[j];
                info.loca[2] = d->sensor[k];
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
        labelDistance tmpDist_1;
        label->RawPoints.append(calcPosFromDistance(dist[i].distance, 4));

        if (i >= 1) {
            /* distance filter BEGIN */
            dType factor = 1.7f;
            dType ratio  = 0.3f;
            dType maxDist = 0.0f;
            dType avgDist_noMax = 0.0f;
            labelDistance tmpDist;
            labelDistance tmpDist_1;
            for (int j = 0; j < 4; j++) {
                tmpDist.distance[j] = dist[i].distance[j];
            }

            int maxIdx = -1;
            for (int j = 0; j < 4; j++) {
                dType diffDist = dType(dist[i].distance[j]) - dType(distRefined[i-1].distance[j]);
                if (qAbs(maxDist) <= qAbs(diffDist)) {
                    maxDist = diffDist;
                    maxIdx = j;
                }
                avgDist_noMax += diffDist;
            }
            avgDist_noMax = (avgDist_noMax - maxDist) / 3.0f;
            if (maxDist > avgDist_noMax * factor) {
                tmpDist.distance[maxIdx] = distRefined[i-1].distance[maxIdx] + maxDist * ratio + avgDist_noMax * (1.f-ratio);
                qDebug() << i << tmpDist.distance[maxIdx] << "##"
                         << maxDist * ratio;
            }
            label->RefinedPoints.append(calcPosFromDistance(tmpDist.distance, 4));
            distRefined.append(tmpDist);
            for (int j = 0; j < 4; j++) {
                tmpDist_1.distance[j] = tmpDist.distance[j];
            }

            qDebug() << i
                     << dist[i].toStringDist()
                     << tmpDist.toStringDist()
                     << labelDistance::diffDist(dist[i], tmpDist).toStringDist()
                     << "maxIdx" << maxIdx
                     << avgDist_noMax << maxDist
                     << (maxDist > avgDist_noMax * factor);

            /* distance filter END */
        } else {
            for (int j = 0; j < 4; j++) {
                tmpDist_1.distance[j] = dist[i].distance[j];
            }
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
    dType maxDistance = 0.0f;
    dType tmpDistance = 0.0f;
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
            center = center + calcPoints[i][j] / dType(calcPoints[i].count());
        }
        optimizedPos = center;
        dType totalDistance = 0.0f;
        dType avgDistance = 0.0f;
        for(int i1 = 0; i1 < rawPoints; i1++) {
            for(int i2 = i1; i2 < rawPoints; i2++) {
                totalDistance += calcDistance(calcPoints[i][i1], calcPoints[i][i2]);
            }
        }
        avgDistance = totalDistance/(rawPoints*(rawPoints-1)/2.0f);
        label->Ans.append(optimizedPos);
        label->Reliability.append(avgDistance);

        if (i > 0)
            label->AnsLines.append(QLine(p_1.toQPoint(), optimizedPos.toQPoint()));
        p_1 = optimizedPos;
        center = {0, 0, 0};
    }
}

void calcPos::calcPosVectorWylie (labelInfo *label) {
    dType historyDist[4][4] = {0.0f};
    int historyCount = 0;
    dType *B = new dType[4];
    dType *X = new dType[4];
    dType *A_data = new dType[12];
    dType **A = new dType*[4];
    A[0] = A_data;
    A[1] = A_data+3;
    A[2] = A_data+6;
    A[3] = A_data+9;

    if (nullptr == label)
        return;

    label->RawPoints.clear();
    for (int i = 0; i < dist.count(); i++) {
        label->RawPoints.append(calcPosFromDistance(dist[i].distance, 4));
        labelDistance tmpDist;

        if (i >= 4) {
            // 循环4个anchor与tag的测量距离
            for (int j = 0; j < 4; j++) {
                // 赋初值
                for (int row = 0; row < 4; row++) {
                    A[row][0] = 1;
                    A[row][1] = row;
                    A[row][2] = row * row;
                    B[row] = historyDist[j][row];
                    X[row] = 0.0f;
                }
                // 用最小二乘法，计算距离的插值曲线
                leastSquare(A, B, X, 4, 3);
                dType L_hat = X[0] + X[1]*4.f + X[2]*16.f;
                //dType percent = qAbs((L_hat-dist[i].distance[j])/dist[i].distance[j]);
                dType sigma = qSqrt((qPow(historyDist[j][0]-X[0], 2)
                                    + qPow(historyDist[j][1]-X[0]-X[1]-X[2], 2)
                                    + qPow(historyDist[j][2]-X[0]-2.f*X[1]-4.f*X[2], 2)
                                    + qPow(historyDist[j][2]-X[0]-3.f*X[1]-9.f*X[2], 2)
                                    + qPow(dist[i].distance[j]-L_hat, 2))/5.0f);
                qDebug() << "i" << i << "X=[" << X[0] << X[1] << X[2] << "], L_hat=" << L_hat
                         << "meas=" << dist[i].distance[j] << "sigma:" << sigma;

                if (sigma < 25) {
                    tmpDist.distance[j] = dist[i].distance[j];
                } else {
                    tmpDist.distance[j] = L_hat * 0.3 + dist[i].distance[j] * 0.7;
                }

                for (int k = 0; k < 3; k++) {
                    historyDist[j][k] = historyDist[j][k+1];
                }
                historyDist[j][3] = dType(dist[i].distance[j]);
            }
            label->RefinedPoints.append(calcPosFromDistance(tmpDist.distance, 4));
            distRefined.append(tmpDist);
        } else {
            for (int j = 0; j < 4; j++) {
                historyDist[j][historyCount] = dType(dist[i].distance[j]);
            }
            historyCount ++;
            label->RefinedPoints.append(calcPosFromDistance(dist[i].distance, 4));
            distRefined.append(dist[i]);
        }
    }
}

void calcPos::calcPotimizedPosWylie(labelInfo *label) {
    Q_UNUSED(label);
}
