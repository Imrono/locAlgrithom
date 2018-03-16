#include "calcTagPos.h"
#include <QtMath>
#include "calcLibMatrixOp.h"
#include "calcLibMath.h"
extern "C" {
#include "armVersion/calcTagLoc_ARM.h"
}

calcTagPos::~calcTagPos() {
    resetA();
}
void calcTagPos::resetA() {
    // Weighted
    if (nullptr == W_taylor)    delete []W_taylor;
    W_taylor = nullptr;

    // Taylor
    if (nullptr != A_taylor) {
        for (int i = 0; i < fc_row; i++) {
            delete []A_taylor[i];
        }
        delete []A_taylor;
        A_taylor = nullptr;
    }
    if (nullptr == B_taylor)    delete []B_taylor;
    B_taylor = nullptr;

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
        qDebug() << "[@calcTagPos::setConfigData] nullptr == cfg_q @ calcTagPos::setConfigData";
        return;
    }
    this->cfg_d = cfg_q;
    resetA();

    if (0 == cfg_d->sensor.count())
        return;

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
    qDebug() << "[@calcTagPos::setConfigData] A_ls => row:" << ls_row <<  "col:" << ls_col;
    //for (int i = 0; i < ls_row; i++) {
    //    qDebug() << A_ls[i][0] << A_ls[i][1];
    //}
    coefficient_B(A_ls, A_ls_inverse_AT, ls_row, ls_col);
    qDebug() << "[@calcTagPos::setConfigData] A_ls_inverse_AT: => row:" << ls_col <<  "col:" << ls_row;
    //for (int i = 0; i < ls_row; i++) {
    //    qDebug() << A_ls_inverse_AT[0][i] << A_ls_inverse_AT[1][i];
    //}

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

    // Taylor Series
    B_taylor = new dType[fc_row];
    A_taylor = new dType* [fc_row];
    for (int i = 0; i < fc_row; i++) {
        A_taylor[i] = new dType[2];
    }

    // weighted
    W_taylor = new dType[fc_row];
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
    if (CALC_POS_TYPE::POS_NONE == calcPosType) {
        return;
    }

    bool isNlosIgnore = false;
    if (CALC_POS_TYPE::WeightedTaylor == calcPosType) {
        isNlosIgnore = false;
    }
    int nSensor = cfg_d->sensor.count();
    locationCoor tmpX = {0.f, 0.f, 0.f};
    dType mse = 0.f;

    QVector<dist4Calc> distRefined;
    bool usedSensor[MAX_SENSOR];
    for (int i = 0; i < MAX_SENSOR; i++)
        usedSensor[i] = true;

    const oneTag &oneTagData = dist_d->tagsData[tagInfo->tagId];
    // used for kalmanTaylor only
    oneKalmanData &kalmanData = tagInfo->calcPosKalmanData;
    tagInfo->isGaussPointAdded = kalmanCoupledType & GAUSS_COUPLED;
    kalmanData.isInitialized = false;

    for (int i = 0; i < oneTagData.distData.count(); i++) {
        locationCoor tmpLastPos = tmpX;
        //qDebug() << i << tmpLastPos.toQPointF();

        // copy distance, nlos distance filter may change the distance
        dist4Calc tmpDist = {{0,0,0,0,0,0}};
        for (int j = 0; j < oneTagData.distData[i].distance.count(); j++) {
            tmpDist.distance[j] = oneTagData.distData[i].distance[j];
        }
        tagInfo->RawPoints.append(calcPosFromDistance(tmpDist.distance, cfg_d->sensor.count()));

        QVector<QPointF> tmpTrace;
        QVector<dType> tmpWeight;
        QPointF x_hat;
        tmpWeight.fill(0.f, 6);
        dType T = oneTagData.distData[i].time.toMSecsSinceEpoch();
        tmpX = calcOnePosition(tmpDist.distance, mse, T, tmpLastPos, kalmanData,
                               usedSensor, tmpTrace, tmpWeight, x_hat);
        if (i >= 1) {
            /* distance filter ITERATION */
            int iterCount = 1;
            while (iterCount-- > 0
                && calcNlos->posPrecisionNLOS(mse)) {   // MSE小于阈值，直接退出循环
                // tmpDist 即是IN也是OUT；distRefined是保存历史修正后的值
                if (calcNlos->pointsPredictNlos(tmpDist, nSensor, distRefined)) {
                    tmpX = calcOnePosition(tmpDist.distance, mse, T, tmpLastPos, kalmanData,
                                           usedSensor, tmpTrace, tmpWeight, x_hat);
                } else {}
            }
            /* distance filter END */
        }

        // crossed circle which measure p's quality with MSE
        int crossed_1 = calcCrossedCircle(oneTagData.distData[i].distance.data(),
                                          cfg_d->sensor.data(), cfg_d->sensor.count(),
                                          tmpX, MACRO_circleR_1);
        int crossed_2 = calcCrossedCircle(oneTagData.distData[i].distance.data(),
                                          cfg_d->sensor.data(), cfg_d->sensor.count(),
                                          tmpX, MACRO_circleR_2);

        // store and update
        distRefined.append(tmpDist);    // used for nlos distance filter
        tagInfo->methodInfo[MEASUR_STR].time.append(oneTagData.distData[i].time);
        tagInfo->RefinedPoints.append(calcPosFromDistance(tmpDist.distance, cfg_d->sensor.count()));
        tagInfo->Reliability.append(mse);
        tagInfo->methodInfo[MEASUR_STR].data[storeMethodInfo::STORED_MSE].append(mse);
        tagInfo->methodInfo[MEASUR_STR].data[storeMethodInfo::STORED_Crossed1].append(crossed_1);
        tagInfo->methodInfo[MEASUR_STR].data[storeMethodInfo::STORED_Crossed2].append(crossed_2);
        tagInfo->methodInfo[MEASUR_STR].Ans.append(tmpX);
        //qDebug() << tmpX.toString();
        //qDebug() << tagInfo->methodInfo[MEASUR_STR].Ans[tagInfo->methodInfo[MEASUR_STR].Ans.count() - 1].toString();
        tagInfo->iterPoints.append(tmpTrace);
        tagInfo->weight.append(tmpWeight);
        if (i > 0) {
            tagInfo->methodInfo[MEASUR_STR].AnsLines
                    .append(QLineF(tagInfo->methodInfo[MEASUR_STR].Ans[i-1].toQPointF(),
                                   tagInfo->methodInfo[MEASUR_STR].Ans[i].toQPointF()));
        }
        QVector<bool> tmp;
        for (int k = 0; k < cfg_d->sensor.count(); k++) {
            tmp.append(usedSensor[k]);
        }
        tagInfo->usedSeneor.append(tmp);
        if (tagInfo->isGaussPointAdded) {
            tagInfo->x_hat.append(x_hat);
        }
    }
}

locationCoor calcTagPos::calcOnePosition(const int *dist, dType &MSE, dType T,
                                         locationCoor lastPos, oneKalmanData &kalmanData,
                                         bool *usedSensor, QVector<QPointF> &iterTrace,
                                         QVector<dType> &weight, QPointF &x_hat) {
    if (CALC_POS_TYPE::FullCentroid == calcPosType) {
        return calcFullCentroid(dist, MSE);
    } else if (CALC_POS_TYPE::SubLS == calcPosType) {
        return calcSubLS(dist, MSE);
    } else if (CALC_POS_TYPE::TwoCenter == calcPosType) {
        return calcTwoCenter(dist, MSE);
    } else if (CALC_POS_TYPE::Taylor == calcPosType) {
        return calcTaylorSeries(dist, MSE);
    } else if (CALC_POS_TYPE::WeightedTaylor == calcPosType) {
        return calcWeightedTaylor(dist, MSE, lastPos, usedSensor, iterTrace, weight);
    }  else if (CALC_POS_TYPE::ARM_calcPos == calcPosType) {
        return calcPos_ARM(dist, MSE, lastPos);
    } else if (CALC_POS_TYPE::LMedS == calcPosType) {
        return calcLMedS(dist, MSE, lastPos, usedSensor, iterTrace);
    } else if (CALC_POS_TYPE::Bilateration == calcPosType) {
        return calcBilateration(dist, MSE);
    } else if (CALC_POS_TYPE::POS_KalmanCoupled== calcPosType
            || CALC_POS_TYPE::POS_KalmanTrail  == calcPosType
            || CALC_POS_TYPE::POS_KalmanGauss  == calcPosType
            || CALC_POS_TYPE::POS_KalmanWeight == calcPosType
            || CALC_POS_TYPE::POS_KalmanSmooth == calcPosType) {
           return calcKalmanCoulped(dist, MSE, T, lastPos, kalmanData, kalmanCoupledType,
                                    usedSensor, iterTrace, weight, x_hat);
    } else if (CALC_POS_TYPE::POS_NONE == calcPosType) {
        return {0.f, 0.f, 0.f};
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
    //qDebug() << N;
    dType X[2] = {0.f};
    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i+1]), 2) - qPow(dType(distance[0]), 2) +
               qPow(sensor[0].x, 2) - qPow(sensor[i+1].x, 2) +
               qPow(sensor[0].y, 2) - qPow(sensor[i+1].y, 2);
    }
    matrixMuti(coA, B, X, 2, N);
    //qDebug() << "CO:" << B[0] << B[1] << B[2] << B[3] << B[4];
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

void calcTagPos::calcTaylorSeries(const int *distance, const locationCoor *sensor,
                                  dType **A, dType **coA, dType *B, int N,
                                  dType **A_taylor, dType *B_taylor,
                                  dType &out_x, dType &out_y, dType &out_MSE) {
    dType X[3] = {0.f};
    dType dX[3] = {0.f};
    dType tmpD = 0.f;
    dType mse = 0.f;
    dType mseKeep = 0.f;
    dType dMse = 10000.f;

    for (int i = 0; i < N; i++) {
        B[i] = qPow(dType(distance[i]), 2) - qPow(sensor[i].x, 2) - qPow(sensor[i].y, 2);
    }
    matrixMuti(coA, B, X, 3, N);
    X[2] = qPow(X[0], 2) + qPow(X[1], 2);
    mse = calcMSE(A, B, X, N, 3);

    int count = 0;
    while (mse > 10000.f && count < 3 && dMse > 1000.f) {
        //qDebug() << count << "calcTaylorSeries" << mse << dMse;
        for (int i = 0; i < N; i++) {
            tmpD = qSqrt(qPow(X[0] - sensor[i].x, 2) + qPow(X[1] - sensor[i].y, 2));
            A_taylor[i][0] = (X[0] - sensor[i].x) / tmpD;
            A_taylor[i][1] = (X[1] - sensor[i].y) / tmpD;
            B_taylor[i] = dType(distance[i]) - tmpD;
        }

        leastSquare(A_taylor, B_taylor, dX, N, 2);

        X[0] += dX[0];
        X[1] += dX[1];
        X[2] = qPow(X[0], 2) + qPow(X[1], 2);

        mseKeep = mse;
        mse = calcMSE(A, B, X, N, 3);
        dMse = qAbs(mseKeep - mse);

        count ++;
    };
    //qDebug() << count << "#calcTaylorSeries#" << mse << dMse;

    // output
    out_x   = X[0];
    out_y   = X[1];
    out_MSE = calcMSE(A, B, X, N, 3);
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

    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcTaylorSeries(const int *dist, dType &MSE) {
    calcTaylorSeries(dist, cfg_d->sensor.data(),
                     A_fc, A_fc_inverse_AT, B_fc, fc_row,
                     A_taylor, B_taylor, X[0], X[1], MSE);

    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcWeightedTaylor(const int *dist, dType &MSE, locationCoor lastPos,
                                            bool *usedSensor, QVector<QPointF> &iterTrace,
                                            QVector<dType> &weight) {
    dType init_W[MAX_SENSOR];
    for (int i = 0; i < MAX_SENSOR; i++) {
        init_W[i] = 1.f;
    }
    calcWeightedTaylor(dist, cfg_d->sensor.data(),
                       lastPos, fc_row, init_W, nullptr,
                       X[0], X[1], MSE,
                       usedSensor, iterTrace, weight);

    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcKalmanCoulped(const int *dist, dType &MSE, dType T, locationCoor lastPos,
                                           oneKalmanData &kalmanData, unsigned int type,
                                           bool *usedSensor, QVector<QPointF> &iterTrace,
                                           QVector<dType> &weight, QPointF &x_hat) {
    Q_UNUSED(lastPos);
    calcKalmanCoulped(dist, cfg_d->sensor.data(), T,
                      kalmanData, fc_row, type,
                      X[0], X[1], MSE,
                      usedSensor, iterTrace, weight, x_hat);
    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcLMedS (const int *dist, dType &MSE, locationCoor lastPos,
                                    bool *usedSensor, QVector<QPointF> &iterTrace) {
    calcLMedS(dist, cfg_d->sensor.data(), lastPos, fc_row, X[0], X[1], MSE,
              usedSensor, iterTrace);
    return locationCoor{X[0], X[1], 0.f};
}
locationCoor calcTagPos::calcBilateration  (const int *dist, dType &MSE) {
    calcBilateration(dist, cfg_d->sensor.data(), fc_row, X[0], X[1], MSE);
    return locationCoor{X[0], X[1], 0.f};
}

locationCoor calcTagPos::calcPos_ARM(const int *dist, dType &MSE, locationCoor lastPos) {
    ST_COL3D data[MAX_SENSOR];
    ST_COL3D lpstCol3DLoc;
    char bInitLocIncluded;

    for (int i = 0; i < fc_row; i++) {
        data[i].fX = cfg_d->sensor[i].x;
        data[i].fY = cfg_d->sensor[i].y;
        data[i].fZ = cfg_d->sensor[i].z;
        data[i].fDistance = dist[i];
    }
    if (qAbs(lastPos.x) < MY_EPS && qAbs(lastPos.y) < MY_EPS) {
        bInitLocIncluded = TRUE;
    } else {
        bInitLocIncluded = FALSE;
    }
    lpstCol3DLoc.fX = lastPos.x;
    lpstCol3DLoc.fY = lastPos.y;
    lpstCol3DLoc.fZ = lastPos.z;

    Cal3DLoc(data, fc_row, fc_row, &lpstCol3DLoc, bInitLocIncluded);

    MSE = MY_EPS;
    return locationCoor{lpstCol3DLoc.fX, lpstCol3DLoc.fY, 0.f};
}
