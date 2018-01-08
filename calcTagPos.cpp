#include "calcTagPos.h"
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
calcTagPos::~calcTagPos() {
    resetA();
}
void calcTagPos::resetA() {
    // FC
    if (nullptr != A_fc) {
        for (int i = 0; i < fc_row; i++) {
            delete []A_fc[i];
        }
        delete []A_fc;
        A_fc = nullptr;
    }
    if (nullptr != A_fc_inverse_AT) {
        for (int i = 0; i < fc_col; i++) {
            delete []A_fc_inverse_AT[i];
        }
        delete []A_fc_inverse_AT;
        A_fc_inverse_AT = nullptr;
    }
    if (nullptr == B_fc)    delete []B_fc;
    B_fc = nullptr;

    // LS
    if (nullptr != A_ls) {
        for (int i = 0; i < ls_row; i++) {
            delete []A_ls[i];
        }
        delete []A_ls;
    }
    A_ls = nullptr;
    if (nullptr != A_ls_inverse_AT) {
        for (int i = 0; i < ls_col; i++) {
            delete []A_ls_inverse_AT[i];
        }
        delete []A_ls_inverse_AT;
        A_ls_inverse_AT = nullptr;
    }
    if (nullptr == B_ls)    delete []B_ls;
    B_ls = nullptr;
}
void calcTagPos::setConfigData(const configData *cfg_q) {
    if (nullptr == cfg_q) {
        qDebug() << "nullptr == cfg_q @ calcTagPos::setConfigData";
        return;
    }
    this->cfg_d = cfg_q;
    resetA();

    // calculate A, (A^T A)^(-1)*A^T
    ls_row = cfg_d->sensor.count() - 1;
    ls_col = 2;
    fc_row = cfg_d->sensor.count();
    fc_col = 3;
    qDebug() << "[@calcTagPos::setConfigData] LS: row=" << ls_row << "col=" << ls_col;
    qDebug() << "[@calcTagPos::setConfigData] FC: row=" << fc_row << "col=" << fc_col;

    // LS PART
    A_ls = new dType *[ls_row];
    for (int i = 0; i < ls_row; i++) {
        A_ls[i] = new dType[2];
        A_ls[i][0] = 2.f*(cfg_d->sensor[0].x - cfg_d->sensor[i+1].x);
        A_ls[i][1] = 2.f*(cfg_d->sensor[0].y - cfg_d->sensor[i+1].y);
    }
    B_ls = new dType[ls_row]{0.f};

    A_ls_inverse_AT = new dType *[ls_col];
    for (int i = 0; i < ls_col; i++) {
        A_ls_inverse_AT[i] = new dType[ls_row];
    }
    qDebug() << "[@calcTagPos::setConfigData] A_ls:" << ls_row << ls_col;
    //qDebug() << A_ls[0][0] << A_ls[0][1]
    //         << A_ls[1][0] << A_ls[1][1]
    //         << A_ls[2][0] << A_ls[2][1];
    coefficient_B(A_ls, A_ls_inverse_AT, ls_row, ls_col);
    qDebug() << "[@calcTagPos::setConfigData] A_ls_inverse_AT:" << ls_col << ls_row;
    //qDebug() << A_ls_inverse_AT[0][0] << A_ls_inverse_AT[0][1] << A_ls_inverse_AT[0][2]
    //         << A_ls_inverse_AT[1][0] << A_ls_inverse_AT[1][1] << A_ls_inverse_AT[1][2];

    // FC PART
    A_fc = new dType *[fc_row];
    for (int i = 0; i < fc_row; i++) {
        A_fc[i] = new dType[3];
        A_fc[i][0] = -2.f*cfg_d->sensor[i].x;
        A_fc[i][1] = -2.f*cfg_d->sensor[i].y;
        A_fc[i][2] = 1.f;
    }
    B_fc = new dType[fc_row];

    A_fc_inverse_AT = new dType *[fc_col];
    for (int i = 0; i < fc_col; i++) {
        A_fc_inverse_AT[i] = new dType[fc_row];
    }
    coefficient_B(A_fc, A_fc_inverse_AT, fc_row, fc_col);
}
void calcTagPos::setDistanceData(const distanceData *dist_q) {
    this->dist_d = dist_q;
}
void calcTagPos::setNlosJudge(const calcTagNLOS *calcNlos) {
    this->calcNlos = calcNlos;
}

locationCoor calcTagPos::calcOnePosFor2Dim(dType dist[], locationCoor loca[]) {
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

QVector<locationCoor> calcTagPos::calcPosFromDistance(const int dist[], uint32_t count) const {
    QVector<locationCoor> vectorAns;
    distance_3 info;
    for (uint32_t i = 0; i < count - 2; i++) {
        for (uint32_t j = i+1; j < count - 1; j++) {
            for (uint32_t k = j+1; k < count; k++) {
                info.dist[0] = dType(dist[i]);
                info.dist[1] = dType(dist[j]);
                info.dist[2] = dType(dist[k]);
                info.loca[0] = cfg_d->sensor[i];
                info.loca[1] = cfg_d->sensor[j];
                info.loca[2] = cfg_d->sensor[k];
                locationCoor pos = calcTagPos::calcOnePosFor2Dim(info.dist, info.loca);
                vectorAns.append(pos);
                //qDebug() << info.toString() << pos.toString();
            }
        }
    }
    //qDebug() << QString("(%0,%1,%2)").arg(x_pos, 4).arg(y_pos, 4).arg(z_pos, 4);
    return vectorAns;
}

void calcTagPos::calcPosVector (storeTagInfo *tagInfo) {
    if (nullptr == tagInfo)
        return;

    int nSensor = cfg_d->sensor.count();
    locationCoor tmpX;
    double mse = 0.f;

    tagInfo->reset(MEASUR_STR);
    distRefined.clear();
    tagInfo->calcPosType = this->calcPosType;

    const oneTag &tagDists = dist_d->tagsData[tagInfo->tagId];
    for (int i = 0; i < tagDists.distData.count(); i++) {
        dist4Calc tmpDist;
        for (int j = 0; j < tagDists.distData[i].distance.count(); j++) {
            tmpDist.distance[j] = tagDists.distData[i].distance[j];
        }
        tagInfo->RawPoints.append(calcPosFromDistance(tmpDist.distance, cfg_d->sensor.count()));

        tmpX = calcOnePosition(tmpDist.distance, mse);
        if (i >= 1) {
            /* distance filter BEGIN */
            // ITERATION
            iterCount = 1;
            do {
                if (!calcNlos->posPrecisionNLOS(mse)) {  // MSE小于阈值，直接退出循环
                    break;
                } else {
                    if (iterCount-- == 0) {
                        break;
                    } else {
                        if (calcNlos->pointsPredictNlos(tmpDist, nSensor, distRefined)) {
                            tmpX = calcOnePosition(tmpDist.distance, mse);
                        } else {}
                    }
                }
            } while(1);
            /* distance filter END */

        // store and update
            tagInfo->methodInfo[MEASUR_STR].Ans.append(tmpX);
            tagInfo->methodInfo[MEASUR_STR].AnsLines
                    .append(QLineF(tagInfo->methodInfo[MEASUR_STR].Ans[i-1].toQPointF(),
                                   tagInfo->methodInfo[MEASUR_STR].Ans[i].toQPointF()));
        } else {
            tagInfo->methodInfo[MEASUR_STR].Ans.append(tmpX);
        }
        tagInfo->RefinedPoints.append(calcPosFromDistance(tmpDist.distance, cfg_d->sensor.count()));
        tagInfo->Reliability.append(mse);
        tagInfo->methodInfo[MEASUR_STR].data[0].append(mse);
        distRefined.append(tmpDist);
    }
}

locationCoor calcTagPos::calcOnePosition(const int *dist, dType &MSE) {
    if (FullCentroid == calcPosType) {
        return calcFullCentroid(dist, MSE);
    } else if (SubLS == calcPosType) {
        return calcSubLS(dist, MSE);
    } else if (TwoCenter == calcPosType) {
        return calcTwoCenter(dist, MSE);
    } else {
        return {0.f, 0.f, 0.f};
    }
}

void calcTagPos::calcFullCentroid(const int *distance, const locationCoor *sensor,
                                  dType **A, dType **coA, dType *B, int N,
                                  dType &out_x, dType &out_y, dType &out_MSE) {
    dType X[3] = {0.f};
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i]), 2) - qPow(sensor[i].x, 2) - qPow(sensor[i].y, 2);
    }
    matrixMuti(coA, B, X, 3, N);
    //qDebug() << X[0] << X[1] << X[2] << X[0]*X[0] + X[1]*X[1];

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, N, 3);
}

void calcTagPos::calcSubLS(const int *distance, const locationCoor *sensor,
                           dType **A, dType **coA, dType *B, int N,
                           dType &out_x, dType &out_y, dType &out_MSE) {
    dType X[2] = {0.f};
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i+1]), 2) - qPow(dType(distance[0]), 2) +
               qPow(sensor[0].x, 2) - qPow(sensor[i+1].x, 2) +
               qPow(sensor[0].y, 2) - qPow(sensor[i+1].y, 2);
    }
    matrixMuti(coA, B, X, 2, N);
    //qDebug() << "CO:" << X[0] << X[1];

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, N, 2);
}

void calcTagPos::calcTwoCenter(const int *distance, const locationCoor *sensor, int N,
                               dType &out_x, dType &out_y, dType &out_MSE) {
    QVector<locationCoor> points;
    dType dist[3] = {0.f};
    locationCoor loca[3] = {{0.f, 0.f, 0.f}};
    locationCoor center = {0.f, 0.f, 0.f};
    dType avgDist = 0.f;
    int nPoint = 0;
    for (int i = 0; i < N - 2; i++) {
        for (int j = i+1; j < N - 1; j++) {
            for (int k = j+1; k < N; k++) {
                dist[0] = dType(distance[i]);
                dist[1] = dType(distance[j]);
                dist[2] = dType(distance[k]);
                loca[0] = sensor[i];
                loca[1] = sensor[j];
                loca[2] = sensor[k];
                locationCoor pos = calcTagPos::calcOnePosFor2Dim(dist, loca);
                points.append(pos);
                //qDebug() << info.toString() << pos.toString();
            }
        }
    }
    nPoint = points.count();
    // get center
    for (int i = 0; i < nPoint; i++) {
        center = center + points[i] / nPoint;
    }

    // average distance as MSE
    for(int i = 0; i < nPoint; i++) {
        avgDist += calcDistance(points[i], center);
    }
    avgDist = avgDist/nPoint;

    // output
    out_x = center.x;
    out_y = center.y;
    out_MSE = avgDist;
}

locationCoor calcTagPos::calcFullCentroid(const int *dist, dType &MSE) {
    calcFullCentroid(dist, cfg_d->sensor.data(),
                     A_fc, A_fc_inverse_AT, B_fc, fc_row, X[0], X[1], MSE);

    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcSubLS(const int *dist, dType &MSE) {
    calcSubLS(dist, cfg_d->sensor.data(),
              A_ls, A_ls_inverse_AT, B_ls, ls_row, X[0], X[1], MSE);

    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcTwoCenter(const int *dist, dType &MSE) {
    calcTwoCenter(dist, cfg_d->sensor.data(),
                  cfg_d->sensor.count(), X[0], X[1], MSE);

    //qDebug() << X[0] << X[1];
    return locationCoor{X[0], X[1], 0.f};
}
