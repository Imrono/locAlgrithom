#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QFileDialog>
#include "calcTargetTracking.h"

uiMainWindow::uiMainWindow(QWidget *parent) :
    QMainWindow(parent), timerStarted(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // CFG DATA
    cfgData.loadNewFile("D:\\code\\kelmanLocationData\\configExample.ini");
    //cfgData.loadNewFile("D:\\code\\kelmanLocationData\\aaa.ini");
    qDebug() << cfgData.toString();
    calcPos.setConfigData(cfgData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());

    // DIST DATA
    distData.loadNewFile_1("D:\\code\\kelmanLocationData\\201712111515.log");
    //distData.loadNewFile_2("D:\\code\\kelmanLocationData\\WC50Y(B)_LOG\\201705181600.log");
    qDebug() << "[@uiMainWindow::uiMainWindow]" << distData.toString();
    foreach (oneTag tag, distData.get_q()->tagsData) {
        store.addNewTagInfo(tag.tagId);
    }
    calcPos.setDistanceData(distData.get_q());
    ui->canvas->setDistanceData(distData.get_q());

    // SET NLOS FOR calcPos
    calcPos.setNlosJudge(&calcNlos);

    checkData();

    // FILE
    connect(ui->actionRead_ini,     SIGNAL(triggered(bool)), this, SLOT(loadIniConfigFile(bool)));
    connect(ui->actionRead_dist,    SIGNAL(triggered(bool)), this, SLOT(loadLogDistanceFile(bool)));
    connect(ui->actionRead_picture, SIGNAL(triggered(bool)), this, SLOT(loadPictureFile(bool)));

    // NLOS
    connect(ui->actionWylie,     SIGNAL(triggered(bool)), this, SLOT(nlosWylie(bool)));
    connect(ui->actionMultiPoint,SIGNAL(triggered(bool)), this, SLOT(nlosMultiPoint(bool)));
    connect(ui->actionRes,       SIGNAL(triggered(bool)), this, SLOT(nlosRes(bool)));
    connect(ui->actionSumDist,   SIGNAL(triggered(bool)), this, SLOT(nlosSumDist(bool)));

    // POSITION
    connect(ui->actionFullCentroid, SIGNAL(triggered(bool)), this, SLOT(posFullCentroid(bool)));
    connect(ui->actionSubLS,        SIGNAL(triggered(bool)), this, SLOT(posSubLS(bool)));
    connect(ui->actionTwoCenter,    SIGNAL(triggered(bool)), this, SLOT(posTwoCenter(bool)));

    // TRACK
    connect(ui->actionKalmanTrack,     SIGNAL(triggered(bool)), this, SLOT(trackKalman(bool)));
    connect(ui->actionkalmanLiteTrack, SIGNAL(triggered(bool)), this, SLOT(trackKalmanLite(bool)));

    /*************************************************************/
    connect(this, SIGNAL(countChanged(int)), ui->canvas, SLOT(followMainWindowCount(int)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

    connect(ui->beginTrack, &QPushButton::clicked, this, [this](void) {
        if (timerStarted) {
            timer.stop();
            this->ui->beginTrack->setText("track");
        } else {
            timer.start(500);
            ui->beginTrack->setText("stop");
        }
        timerStarted = !timerStarted;
    });
    connect(ui->showPath, &QPushButton::clicked, this, [this](void) {
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        ui->showPath->setFont(font);
        if (!ui->canvas->reverseShowPath()) {
            ui->showPath->setText(MY_STR("显示路径"));
        } else {
            ui->showPath->setText(MY_STR("隐藏路径"));
            foreach (storeTagInfo *info, store.tags) {
                qDebug() << "showPath:" << info->tagId
                         << info->methodInfo[MEASUR_STR].AnsLines.count()
                         << info->methodInfo[KALMAN_STR].AnsLines.count();
                ui->canvas->setLines(info->tagId, MEASUR_STR, info->methodInfo[MEASUR_STR].AnsLines);
                ui->canvas->setLines(info->tagId, KALMAN_STR, info->methodInfo[KALMAN_STR].AnsLines);
            }
        }

        update();
    });
    connect(ui->reset, &QPushButton::clicked, this, [this](void) {
        distCount = 0;
        handleTimeout();
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        distCount -= 2;     //handleTimeout()里有distCount++，所以这里-2
        handleTimeout();
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        handleTimeout();
    });
    connect(ui->allPos, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowAllPos()) {
            ui->allPos->setText(MY_STR("显示所有点"));
        } else {
            ui->allPos->setText(MY_STR("隐藏所有点"));
        }

        update();
    });
    connect(ui->showRadius, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowRadius()) {
            ui->showRadius->setText(MY_STR("显示半径"));
        } else {
            ui->showRadius->setText(MY_STR("隐藏半径"));
        }

        update();
    });
    connect(ui->showTrack, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowTrack()) {
            ui->showTrack->setText(MY_STR("显示Track"));
        } else {
            ui->showTrack->setText(MY_STR("隐藏Track"));
        }

        update();
    });

    // initial calculate method
    nlosRes(true);
    nlosMultiPoint(true);
    posSubLS(true);
    trackKalman(true);
}

uiMainWindow::~uiMainWindow()
{
    delete ui;
}

void uiMainWindow::checkData() {
    qDebug() << "cfgData.get_q() =" << cfgData.get_q()->isInitialized
             << "distData.get_q() =" << distData.get_q()->isInitialized;
    if (!cfgData.get_q()->isInitialized || !distData.get_q()->isInitialized) {
        ui->actionWylie->setDisabled(true);
        ui->actionMultiPoint->setDisabled(true);
        ui->actionRes->setDisabled(true);
        ui->actionSumDist->setDisabled(true);

        ui->actionFullCentroid->setDisabled(true);
        ui->actionSubLS->setDisabled(true);
        ui->actionTwoCenter->setDisabled(true);

        ui->actionKalmanTrack->setDisabled(true);
        ui->actionkalmanLiteTrack->setDisabled(true);
    } else {
        ui->actionWylie->setEnabled(true);
        ui->actionMultiPoint->setEnabled(true);
        ui->actionRes->setEnabled(true);
        ui->actionSumDist->setEnabled(true);

        ui->actionFullCentroid->setEnabled(true);
        ui->actionSubLS->setEnabled(true);
        ui->actionTwoCenter->setEnabled(true);

        ui->actionKalmanTrack->setEnabled(true);
        ui->actionkalmanLiteTrack->setEnabled(true);
    }
}

void uiMainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
}

void uiMainWindow::handleTimeout() {
    distCount = distCount < 0 ? 0 : distCount;
    distCount++;    //为了保证qDebug与paintEvent显示一致，先distCount++，实际从1开始。
    emit countChanged(distCount);

    foreach (oneTag tag, distData.get_q()->tagsData) {
        storeTagInfo *oneTagInfo = store.getTagInfo(tag.tagId);

        ui->canvas->setPosition(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF());
        ui->canvas->setLine(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines[distCount-1]);

        ui->canvas->setPointsRaw(tag.tagId, MEASUR_STR, oneTagInfo->RawPoints[distCount]);
        ui->canvas->setPointsRefined(tag.tagId, MEASUR_STR, oneTagInfo->RefinedPoints[distCount]);

        ui->canvas->setDistance(tag.tagId, distData.get_q()->tagsData[tag.tagId].distData[distCount].distance.data());
    }

//    ui->raw_0->setText(QString::number(distData.get_q()->dist[distCount].distance[0]));
//    ui->raw_1->setText(QString::number(distData.get_q()->dist[distCount].distance[1]));
//    ui->raw_2->setText(QString::number(distData.get_q()->dist[distCount].distance[2]));
//    ui->raw_3->setText(QString::number(distData.get_q()->dist[distCount].distance[3]));

//    if (distData.get_q()->dist[distCount].distance[0] != calcPos.distRefined[distCount].distance[0]) {
//        ui->refine_0->setStyleSheet("color:red; font-weight:bold");
//    } else {
//        ui->refine_0->setStyleSheet("");
//    }
//    if (distData.get_q()->dist[distCount].distance[1] != calcPos.distRefined[distCount].distance[1]) {
//        ui->refine_1->setStyleSheet("color:red; font-weight:bold");
//    } else {
//        ui->refine_1->setStyleSheet("");
//    }
//    if (distData.get_q()->dist[distCount].distance[2] != calcPos.distRefined[distCount].distance[2]) {
//        ui->refine_2->setStyleSheet("color:red; font-weight:bold");
//    } else {
//        ui->refine_2->setStyleSheet("");
//    }
//    if (distData.get_q()->dist[distCount].distance[3] != calcPos.distRefined[distCount].distance[3]) {
//        ui->refine_3->setStyleSheet("color:red; font-weight:bold");
//    } else {
//        ui->refine_3->setStyleSheet("");
//    }
//    ui->refine_0->setText(QString::number(calcPos.distRefined[distCount].distance[0]));
//    ui->refine_1->setText(QString::number(calcPos.distRefined[distCount].distance[1]));
//    ui->refine_2->setText(QString::number(calcPos.distRefined[distCount].distance[2]));
//    ui->refine_3->setText(QString::number(calcPos.distRefined[distCount].distance[3]));

    update();

    if (distCount == distData.maxDataCount)
        timer.stop();
}
/***********************************************************/
// MENU ACTION
/***********************************************************/
// FILE
void uiMainWindow::loadIniConfigFile(bool checked) {
    Q_UNUSED(checked);
    QString path = QFileDialog::getOpenFileName(this, "Select Config File", ".", "config file(*.ini)");
    qDebug() << "loadIniConfigFile Path:" << path;
    cfgData.loadNewFile(path);
    calcPos.setConfigData(cfgData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());
    ui->actionRead_ini->setChecked(true);
    checkData();
}
void uiMainWindow::loadLogDistanceFile(bool checked) {
    Q_UNUSED(checked);
    QString path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    qDebug() << "loadLogDistanceFile Path:" << path;
    distData.clear();
    distData.loadNewFile_1(path);
    calcPos.setDistanceData(distData.get_q());
    ui->actionRead_dist->setChecked(true);
    qDebug() << "[@uiMainWindow::loadLogDistanceFile]" << distData.toString();
    checkData();
}
void uiMainWindow::loadPictureFile(bool checked) {
    Q_UNUSED(checked);
    QString path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "picture file(*.*)");
    qDebug() << "loadPictureFile Path:" << path;
    ui->canvas->loadPicture(path);
    ui->actionRead_dist->setChecked(true);
}

// NLOS
void uiMainWindow::nlosWylie(bool checked) {
    Q_UNUSED(checked);
    if (calcNlos.predictNlos != POINTS_NLOS::WYLIE) {
        ui->actionWylie->setChecked(true);
        calcNlos.predictNlos = POINTS_NLOS::WYLIE;
    } else {
        ui->actionWylie->setChecked(false);
        calcNlos.predictNlos = POINTS_NLOS::POINTS_NONE;
    }
    ui->actionMultiPoint->setChecked(false);
    qDebug() << "nlosWylie :" << calcNlos.predictNlos;
}

void uiMainWindow::nlosMultiPoint(bool checked) {
    Q_UNUSED(checked);
    ui->actionWylie->setChecked(false);
    if (calcNlos.predictNlos != POINTS_NLOS::MULTI_POINT) {
        calcNlos.predictNlos = POINTS_NLOS::MULTI_POINT;
        ui->actionMultiPoint->setChecked(true);
    } else {
        calcNlos.predictNlos = POINTS_NLOS::POINTS_NONE;
        ui->actionMultiPoint->setChecked(false);
    }
    qDebug() << "nlosMultiPoint :" << calcNlos.predictNlos;
}

void uiMainWindow::nlosRes(bool checked) {
    Q_UNUSED(checked);
    ui->actionSumDist->setChecked(false);
    if (calcNlos.precNlos != POS_PRECISION_NLOS::RESIDUAL) {
        calcNlos.precNlos = POS_PRECISION_NLOS::RESIDUAL;
        ui->actionRes->setChecked(true);
    } else {
        calcNlos.precNlos = POS_PRECISION_NLOS::POS_PRECISION_NONE;
        ui->actionRes->setChecked(false);
    }
    qDebug() << "nlosRes :" << calcNlos.precNlos;
}

void uiMainWindow::nlosSumDist(bool checked) {
    Q_UNUSED(checked);
    ui->actionRes->setChecked(false);
    if (calcNlos.precNlos != POS_PRECISION_NLOS::SUM_DIST) {
        calcNlos.precNlos = POS_PRECISION_NLOS::SUM_DIST;
        ui->actionSumDist->setChecked(true);
    } else {
        calcNlos.precNlos = POS_PRECISION_NLOS::POS_PRECISION_NONE;
        ui->actionSumDist->setChecked(false);
    }
    qDebug() << "nlosSumDist :" << calcNlos.precNlos;
}

// POSITION
void uiMainWindow::posFullCentroid(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(true);
    ui->actionSubLS->setChecked(false);
    ui->actionTwoCenter->setChecked(false);
    calcPos.calcPosType = CALC_POS_TYPE::FullCentroid;

    foreach (storeTagInfo *info, store.tags) {
        info->reset(MEASUR_STR);
        calcPos.calcPosVector(store.getTagInfo(info->tagId));

        qDebug() << "posFullCentroid:" << info->toString();
        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "posFullCentroid: avgDistanceSquare => measDist:" << measDist;

        ui->canvas->setLines(info->tagId, MEASUR_STR, info->methodInfo[MEASUR_STR].AnsLines);
    }

    update();
}

void uiMainWindow::posSubLS(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(false);
    ui->actionSubLS->setChecked(true);
    ui->actionTwoCenter->setChecked(false);
    calcPos.calcPosType = CALC_POS_TYPE::SubLS;

    foreach (storeTagInfo *info, store.tags) {
        qDebug() << "posSubLS: tmp " << info->tagId << info->methodInfo.count();
        if (info->methodInfo.contains(MEASUR_STR)) {
            info->reset(MEASUR_STR);
        } else {
            info->methodInfo.insert(MEASUR_STR, storeMethodInfo(METHOD_SUB_LS_STR, info));
        }
        info->methodInfo[MEASUR_STR].methodName = METHOD_SUB_LS_STR;
        qDebug() << "posSubLS: tmp1 " << info->tagId << info->methodInfo.count() << info->methodInfo[MEASUR_STR].AnsLines.count();
        calcPos.calcPosVector(info);
        qDebug() << "posSubLS: tmp2 " << info->tagId << info->methodInfo.count() << info->methodInfo[MEASUR_STR].AnsLines.count();

        qDebug() << "posSubLS:" << info->toString();
        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "posSubLS: avgDistanceSquare => measDist:" << measDist << info->methodInfo[MEASUR_STR].AnsLines.count();

        ui->canvas->setLines(info->tagId, MEASUR_STR, info->methodInfo[MEASUR_STR].AnsLines);
    }

    update();
}

void uiMainWindow::posTwoCenter(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(false);
    ui->actionSubLS->setChecked(false);
    ui->actionTwoCenter->setChecked(true);
    calcPos.calcPosType = CALC_POS_TYPE::TwoCenter;

    foreach (storeTagInfo *info, store.tags) {
        info->reset(MEASUR_STR);
        calcPos.calcPosVector(store.getTagInfo(info->tagId));

        qDebug() << "posTwoCenter:" << info->toString();
        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "posTwoCenter: avgDistanceSquare => measDist:" << measDist;

        ui->canvas->setLines(info->tagId, MEASUR_STR, info->methodInfo[MEASUR_STR].AnsLines);
    }

    update();
}

// TRACK
void uiMainWindow::trackKalman(bool checked) {
    Q_UNUSED(checked);
    ui->actionKalmanTrack->setChecked(true);
    ui->actionkalmanLiteTrack->setChecked(false);

    foreach (storeTagInfo *info, store.tags) {
        if (info->methodInfo.contains(KALMAN_STR)) {
            info->reset(KALMAN_STR);
        } else {
            info->methodInfo.insert(KALMAN_STR, storeMethodInfo(METHOD_KALMAN_STR, info));
        }
        info->methodInfo[KALMAN_STR].methodName = METHOD_KALMAN_STR;

        calcKalman::calcKalmanPosVector(info->methodInfo[MEASUR_STR], info->methodInfo[KALMAN_STR]);

        qDebug() << info->toString();
        dType measDist   = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        dType kalmanDist = calcTotalAvgDistanceSquare(info->methodInfo[KALMAN_STR].AnsLines);
        qDebug() << "kalmanTrackLite: avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

        ui->canvas->setLines(info->tagId, KALMAN_STR, info->methodInfo[KALMAN_STR].AnsLines);
    }
    update();
}
void uiMainWindow::trackKalmanLite(bool checked) {
    Q_UNUSED(checked);
    ui->actionKalmanTrack->setChecked(false);
    ui->actionkalmanLiteTrack->setChecked(true);

    foreach (storeTagInfo *info, store.tags) {
        info->clear();
        calcKalman::calcKalmanPosVectorLite(info->methodInfo[KALMAN_STR], info->methodInfo[MEASUR_STR]);

        qDebug() << info->toString();
        dType measDist   = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        dType kalmanDist = calcTotalAvgDistanceSquare(info->methodInfo[KALMAN_STR].AnsLines);
        qDebug() << "kalmanTrackLite: avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

        ui->canvas->setLines(info->tagId, KALMAN_STR, info->methodInfo[KALMAN_STR].AnsLines);
    }

    update();
}
