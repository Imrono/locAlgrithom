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
    distCountShow = new QLabel(this);
    statusBar()->addWidget(distCountShow);
    distCountShow->setText(QString("distCount: %0").arg(0, 4, 10, QChar('0')));

    // CFG DATA
    //cfgData.loadNewFile("D:\\code\\kelmanLocationData\\configExample.ini");
    cfgData.loadNewFile("D:\\code\\kelmanLocationData\\aaa.ini");
    qDebug() << cfgData.toString();
    calcPos.setConfigData(cfgData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());
    ui->canvas->syncWithUiFrame(ui->UsrFrm);

    // DIST DATA
    //distData.loadNewFile_1("D:\\code\\kelmanLocationData\\201712111515.log");
    //distData.loadNewFile_2("D:\\code\\kelmanLocationData\\WC50Y(B)_LOG\\201705181600.log");
    distData.loadNewFile_2("D:\\code\\kelmanLocationData\\WC50Y(B)_LOG\\201705191135.log");
    qDebug() << "[@uiMainWindow::uiMainWindow]" << distData.toString();
    foreach (oneTag tag, distData.get_q()->tagsData) {
        store.addNewTagInfo(tag.tagId);
        ui->UsrFrm->addOneUsr(tag.tagId, USR_STATUS::HAS_DISTANCE_DATA);
    }
    calcPos.setDistanceData(distData.get_q());
    ui->canvas->setDistanceData(distData.get_q());
    ui->canvas->syncWithUiFrame(ui->UsrFrm);

    // SET NLOS FOR calcPos
    calcPos.setNlosJudge(&calcNlos);

    checkData();

    // FILE
    connect(ui->actionRead_ini,     SIGNAL(triggered(bool)), this, SLOT(loadIniConfigFile(bool)));
    connect(ui->actionRead_dist,    SIGNAL(triggered(bool)), this, SLOT(loadLogDistanceFile(bool)));
    connect(ui->actionRead_dist_2,  SIGNAL(triggered(bool)), this, SLOT(loadLogDistanceFile_2(bool)));
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
        ui->showPath->setStyleSheet("font-weight: bold;");
        if (!ui->canvas->reverseShowPath()) {
            ui->showPath->setText(MY_STR("显示路径"));
        } else {
            ui->showPath->setText(MY_STR("隐藏路径"));
        }
        handleTimeout(false);
        update();
    });
    connect(ui->reset, &QPushButton::clicked, this, [this](void) {
        distCount = 1;
        handleTimeout(false);
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        distCount --;
        handleTimeout(false);
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        distCount ++;
        handleTimeout(false);
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

    connect(ui->UsrFrm, SIGNAL(oneUsrBtnClicked_siganl(int, bool)), this, SLOT(oneUsrBtnClicked(int, bool)));

    // initial calculate method
    nlosRes(true);
    nlosMultiPoint(true);
    posSubLS(true);
    trackKalman(true);

    handleTimeout(false);
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

void uiMainWindow::oneUsrBtnClicked(int tagId, bool isShowable) {
    Q_UNUSED(tagId);
    Q_UNUSED(isShowable);
    ui->canvas->syncWithUiFrame(ui->UsrFrm);
    handleTimeout(false);
}

void uiMainWindow::handleTimeout(bool isUpdateCount) {
    distCount = distCount < 1 ? 1 : distCount;
    if (isUpdateCount) {
        distCount++;    //为了保证qDebug与paintEvent显示一致，先distCount++，实际从1开始。
    }
    distCountShow->setText(QString("distCount: %0").arg(distCount, 4, 10, QChar('0')));

    foreach (oneTag tag, distData.get_q()->tagsData) {
        if (ui->UsrFrm->isShowable(tag.tagId)) {
            storeTagInfo *oneTagInfo = store.getTagInfo(tag.tagId);

            ui->canvas->setPosition(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF());
            ui->canvas->setPosition(tag.tagId, KALMAN_STR, tag.distData[distCount].p_t.toQPointF());
            //qDebug() << calcDistance(tag.distData[distCount].p_t.toQPointF(),
            //                         oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF());

            //ui->canvas->setLine(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines[distCount-1]);

            //ui->canvas->setPosition(tag.tagId, KALMAN_STR, oneTagInfo->methodInfo[KALMAN_STR].Ans[distCount].toQPointF());
            //ui->canvas->setLine(tag.tagId, KALMAN_STR, oneTagInfo->methodInfo[KALMAN_STR].AnsLines[distCount-1]);

            ui->canvas->setPointsRaw(tag.tagId, MEASUR_STR, oneTagInfo->RawPoints[distCount]);
            ui->canvas->setPointsRefined(tag.tagId, MEASUR_STR, oneTagInfo->RefinedPoints[distCount]);

            ui->canvas->setDistance(tag.tagId, distData.get_q()->tagsData[tag.tagId].distData[distCount].distance.data());

            ui->canvas->setLines(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines);
            ui->canvas->setLines(tag.tagId, KALMAN_STR, oneTagInfo->methodInfo[KALMAN_STR].AnsLines);

            switch (tag.distData[distCount].distance.count()) {
            case 6:ui->raw_5->setText(QString::number(tag.distData[distCount].distance[5]));
            case 5:ui->raw_4->setText(QString::number(tag.distData[distCount].distance[4]));
            case 4:ui->raw_3->setText(QString::number(tag.distData[distCount].distance[3]));
            case 3:ui->raw_2->setText(QString::number(tag.distData[distCount].distance[2]));
            case 2:ui->raw_1->setText(QString::number(tag.distData[distCount].distance[1]));
            case 1:ui->raw_0->setText(QString::number(tag.distData[distCount].distance[0]));
            default:
                break;
            }

            switch (tag.distData[distCount].distance.count()) {
            case 6:ui->refine_5->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[5])));
            case 5:ui->refine_4->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[4])));
            case 4:ui->refine_3->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[3])));
            case 3:ui->refine_2->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[2])));
            case 2:ui->refine_1->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[1])));
            case 1:ui->refine_0->setText(QString::number(calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[0])));
            default:
                break;
            }
        } else {
            ui->canvas->clearData(tag.tagId);
        }
    }

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

    // CLEAR LEGACY
    store.clear();
    distData.clear();
    ui->UsrFrm->removeAll();
    ui->canvas->removeAll();

    distData.loadNewFile_1(path);
    foreach (oneTag tag, distData.get_q()->tagsData) {
        store.addNewTagInfo(tag.tagId);
        ui->UsrFrm->addOneUsr(tag.tagId, USR_STATUS::HAS_DISTANCE_DATA);
    }
    calcPos.setDistanceData(distData.get_q());
    ui->canvas->setDistanceData(distData.get_q());
    ui->canvas->syncWithUiFrame(ui->UsrFrm);

    ui->actionRead_dist->setChecked(true);
    ui->actionRead_dist_2->setChecked(false);
    qDebug() << "[@uiMainWindow::loadLogDistanceFile]" << distData.toString();
    checkData();
}
void uiMainWindow::loadLogDistanceFile_2(bool checked) {
    Q_UNUSED(checked);
    QString path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    qDebug() << "loadLogDistanceFile_2 Path:" << path;

    // CLEAR LEGACY
    store.clear();
    distData.clear();
    ui->UsrFrm->removeAll();
    ui->canvas->removeAll();

    distData.loadNewFile_2(path);
    foreach (oneTag tag, distData.get_q()->tagsData) {
        store.addNewTagInfo(tag.tagId);
        ui->UsrFrm->addOneUsr(tag.tagId, USR_STATUS::HAS_DISTANCE_DATA);
    }
    calcPos.setDistanceData(distData.get_q());
    ui->canvas->setDistanceData(distData.get_q());
    ui->canvas->syncWithUiFrame(ui->UsrFrm);

    ui->actionRead_dist->setChecked(false);
    ui->actionRead_dist_2->setChecked(true);
    qDebug() << "[@uiMainWindow::loadLogDistanceFile_2]" << distData.toString();
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
        info->addOrResetMethodInfo(MEASUR_STR, METHOD_FULL_CENTROID_STR);
        calcPos.calcPosVector(store.getTagInfo(info->tagId));
        ui->UsrFrm->setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);

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
        info->addOrResetMethodInfo(MEASUR_STR, METHOD_SUB_LS_STR);
        calcPos.calcPosVector(info);
        ui->UsrFrm->setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);

        qDebug() << "[@posSubLS]" << info->toString();
        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "[@posSubLS] avgDistanceSquare => measDist:" << measDist << info->methodInfo[MEASUR_STR].AnsLines.count();

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
        info->addOrResetMethodInfo(MEASUR_STR, METHOD_TWO_CENTER_STR);
        calcPos.calcPosVector(store.getTagInfo(info->tagId));
        ui->UsrFrm->setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);

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
        info->addOrResetMethodInfo(KALMAN_STR, METHOD_KALMAN_STR);
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
        info->addOrResetMethodInfo(KALMAN_STR, METHOD_KALMAN_LITE_STR);
        calcKalman::calcKalmanPosVectorLite(info->methodInfo[KALMAN_STR], info->methodInfo[MEASUR_STR]);

        qDebug() << info->toString();
        dType measDist   = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        dType kalmanDist = calcTotalAvgDistanceSquare(info->methodInfo[KALMAN_STR].AnsLines);
        qDebug() << "kalmanTrackLite: avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

        ui->canvas->setLines(info->tagId, KALMAN_STR, info->methodInfo[KALMAN_STR].AnsLines);
    }

    update();
}
