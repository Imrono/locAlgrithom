#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QFileDialog>
#include "calcTagTrack.h"

uiMainWindow::uiMainWindow(QWidget *parent) :
    QMainWindow(parent), timerStarted(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon/../resource/icon/locAlg.png"));
    ui->previous->setToolTip("Shift + <");
    ui->next->setToolTip("Shift + >");

    // status bar shows the zoom
    distZoomShow = new QLabel(this);
    statusBar()->addWidget(distZoomShow);
    setStatusZoom();
    distZoomShow->setToolTip("Ctrl + wheelUp/Down");
    // status bar canvas pos show
    canvasPosShow = new QLabel(this);
    statusBar()->addWidget(canvasPosShow);
    setStatusMousePos(0, 0);
    // status bar shows the "distCount"
    distCountShow = new QLabel(this);
    statusBar()->addWidget(distCountShow);
    setStatusDistCount();
    // status bar shows the "calcTimeElapsed"
    calcTimeElapsed = new QLabel(this);
    calcTimeElapsed->setAlignment(Qt::AlignRight);
    calcTimeElapsed->setFrameStyle(QFrame::Box | QFrame::Sunken);
    statusBar()->addPermanentWidget(calcTimeElapsed);
    setStatusTimeInfo();
    iterationNum = new QLabel(this);
    statusBar()->addWidget(iterationNum);
    setStatusIter(0, 0.f);

    // CFG DATA
    //loadIniConfigFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/config/WC50Y(B)型支架运输车.ini"));
    loadIniConfigFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/config/石煤测试5.ini"));
    //loadIniConfigFile(true, "D:\\code\\kelmanLocationData\\configExample.ini");

    // DIST DATA
    //loadLogDistanceFile_2(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/distance/201705181600.log"));
    //loadLogDistanceFile_2(true, "D:\\code\\kelmanLocationData\\WC50Y(B)_LOG\\201705191135.log");
    loadLogDistanceFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/distance/201712201435.log"));
    //loadLogDistanceFile(true, "D:\\code\\kelmanLocationData\\201712111515.log");

    // SET NLOS FOR calcPos
    calcPos.setNlosJudge(&calcNlos);

    checkData();

    // connect signals and slots for uiMainWindow
    connectUi();

    // initial calculate method
    //nlosRes(true);
    //nlosMultiPoint(true);
    posWeightedTaylor(true);
    //trackKalman(true);

    handleModelDataUpdate(false);
}

uiMainWindow::~uiMainWindow()
{
    delete ui;
}

void uiMainWindow::oneUsrBtnClicked(int tagId, bool isShowable) {
    Q_UNUSED(tagId);
    Q_UNUSED(isShowable);
    ui->canvas->syncWithUiFrame(ui->UsrFrm);
    handleModelDataUpdate(false);
}

void uiMainWindow::handleModelDataUpdate(bool isUpdateCount) {
    distCount = distCount < 1 ? 1 : distCount;
    // distCount updated automaticly when timeout event occurs
    if (isUpdateCount) {
        distCount++;
    }
    // update the status bar info
    setStatusDistCount();

    // prepare model data for canvas view
    foreach (oneTag tag, distData.get_q()->tagsData) {
        storeTagInfo *oneTagInfo = store.getTagInfo(tag.tagId);

        // CONDITIONs to the data
        // 1. the user wants to show
        if (ui->UsrFrm->isShowable(tag.tagId)
        // 2. MEASURE (position) is sucessful processed
         && oneTagInfo->isTagPosInitialed
        // 3. for multi-tag, tags data count may different
         && oneTagInfo->methodInfo[MEASUR_STR].AnsLines.count() > distCount) {

            if (CALC_POS_TYPE::POS_NONE != calcPos.calcPosType) {
                ui->canvas->setPosition(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF());
                ui->canvas->setLine(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines[distCount-1]);
            } else {}
            //QPointF tmpOK = tag.distData[distCount].p_t.toQPointF();
            //tmpOK = QPointF(ui->canvas->widthActual, ui->canvas->heightActual) - tag.distData[distCount].p_t.toQPointF();
            //ui->canvas->setPosition(tag.tagId, TRACKx_STR, tmpOK);
            //ui->canvas->setPosition(tag.tagId, TRACKx_STR, tag.distData[distCount].p_t.toQPointF());
            /*
            QVector<QLineF> tmpLines;
            for (int i = 0; i < tag.distData.count(); i++) {
                tmpLines.append(tag.distData[i].l_t);
            }
            ui->canvas->setLines(tag.tagId, TRACKx_STR, tmpLines);
            */
			qDebug() << distCount
				<< oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF()
				<< calcDistance(tag.distData[distCount].p_t.toQPointF(),
					oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF());
                    // << oneTagInfo->methodInfo[TRACKx_STR].Ans[distCount].toQPointF();
            //qDebug() << "[@handleModelDataUpdate] " << distCount << tag.tagId << MEASUR_STR << oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF();
            if (TRACK_METHOD::TRACK_NONE != calcTrack.calcTrackMethod) {
                ui->canvas->setPosition(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].Ans[distCount].toQPointF());
                ui->canvas->setLine(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].AnsLines[distCount-1]);
            } else {}

            ui->canvas->setPointsRaw(tag.tagId, MEASUR_STR, oneTagInfo->RawPoints[distCount]);
            ui->canvas->setPointsRefined(tag.tagId, MEASUR_STR, oneTagInfo->RefinedPoints[distCount]);

            ui->canvas->setDistance(tag.tagId,
                                    distData.get_q()->tagsData[tag.tagId].distData[distCount].distance.data(),
                                    oneTagInfo->usedSeneor[distCount].data());

            ui->canvas->setIterPoints(tag.tagId, oneTagInfo->iterPoints[distCount]);
            setStatusIter(oneTagInfo->iterPoints[distCount].count(),
                          oneTagInfo->methodInfo[MEASUR_STR].data[0][distCount]);

            ui->canvas->setLines(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines);
            ui->canvas->setLines(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].AnsLines);

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
void uiMainWindow::loadIniConfigFile(bool checked, QString pathIn) {
    Q_UNUSED(checked);
    resetData();

    int nSensorKeep = cfgData.get_q()->sensor.count();
    bool isInitKeep = cfgData.get_q()->isInitialized;
    QString path;
    if (0 == pathIn.length()) {
        path = QFileDialog::getOpenFileName(this, "Select Config File", ".", "config file(*.ini)");
    } else {
        path = pathIn;
    }
    qDebug() << "[@uiMainWindow::loadIniConfigFile] Path:" << path;
    cfgData.loadNewFile(path);
    calcPos.setConfigData(cfgData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());
    ui->actionRead_ini->setChecked(true);

    checkData();

    if (isInitKeep && nSensorKeep != cfgData.get_q()->sensor.count()) {
        qDebug() << "[@uiMainWindow::loadIniConfigFile] nSensor changed, need to reload distance file";
        loadLogDistanceFile_2(true);
    }
}
void uiMainWindow::loadLogDistanceFile(bool checked, QString pathIn) {
    Q_UNUSED(checked);
    resetData();

    QString path;
    if (0 == pathIn.length()) {
        path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    } else {
        path = pathIn;
    }
    qDebug() << "[@uiMainWindow::loadLogDistanceFile] Path:" << path;

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
void uiMainWindow::loadLogDistanceFile_2(bool checked, QString pathIn) {
    Q_UNUSED(checked);
    resetData();

    QString path;
    if (0 == pathIn.length()) {
        path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    } else {
        path = pathIn;
    }
    qDebug() << "[@uiMainWindow::loadLogDistanceFile_2] Path:" << path;

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
void uiMainWindow::loadPictureFile(bool checked, QString pathIn) {
    Q_UNUSED(checked);
    QString path;
    if (0 == pathIn.length()) {
        path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "picture file(*.*)");
    } else {
        path = pathIn;
    }
    qDebug() << "[@uiMainWindow::loadPictureFile] Path:" << path;
    ui->canvas->loadPicture(path);
    ui->actionRead_dist->setChecked(true);
}
/***********************************************************/
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
    qDebug() << "[@nlosWylie] clicked";
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
    qDebug() << "[@nlosMultiPoint] clicked";
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
    qDebug() << "[@nlosRes] clicked";
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
    qDebug() << "[@nlosSumDist] clicked";
}

/***********************************************************/
// POSITION
void uiMainWindow::posCalcPROCESS(CALC_POS_TYPE type) {
    resetUi(true, false);
    if (CALC_POS_TYPE::FullCentroid == type) {
        ui->actionFullCentroid->setChecked(true);
    } else if (CALC_POS_TYPE::SubLS == type) {
        ui->actionSubLS->setChecked(true);
    } else if (CALC_POS_TYPE::TwoCenter == type) {
        ui->actionTwoCenter->setChecked(true);
    } else if (CALC_POS_TYPE::Taylor == type) {
        ui->actionTaylorSeries->setChecked(true);
    } else if (CALC_POS_TYPE::WeightedTaylor == type) {
        ui->actionWeightedTaylor->setChecked(true);
    } else if (CALC_POS_TYPE::KalmanTaylor == type) {
        ui->actionKalmanTaylor->setChecked(true);
    } else if (CALC_POS_TYPE::ARM_calcPos == type) {
        ui->actioncalcTagPos_ARM->setChecked(true);
    } else {}

    // determine the calculate method
    calcPos.calcPosType = type;

    QTime time;
    time.start();
    totalPos = 0;
    foreach (storeTagInfo *info, store.tags) {
        ui->canvas->clearData(info->tagId);
        info->clear();
        info->addOrResetMethodInfo(MEASUR_STR, CALC_POS2STR[type]);
        info->calcPosType = type;
/****** CALC POS MAIN **************************************/
        calcPos.calcPosVector(info);
/***********************************************************/
        ui->UsrFrm->setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);
        info->isTagPosInitialed = true;
        info->reset(TRACKx_STR);
        totalPos += info->methodInfo[MEASUR_STR].Ans.count();

        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type] << info->toString();
        qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type]
                 << "avgDistanceSquare => measDist:" << measDist
                 << info->methodInfo[MEASUR_STR].AnsLines.count();
    }
    calcTimeElapsedMeasu = time.elapsed()/1000.f;
    calcTimeElapsedTrack = 0.f;
    setStatusTimeInfo();
    qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type]
             << "total Pos:" << totalPos << ";"
             << "using Time:" << calcTimeElapsedMeasu << "(s)";

    ui->actionKalmanTrack->setChecked(false);
    ui->actionKalmanLiteTrack->setChecked(false);
    ui->actionKalmanInfoTrack->setChecked(false);
    calcTrack.calcTrackMethod = TRACK_METHOD::TRACK_NONE;

    handleModelDataUpdate(false);
}

void uiMainWindow::posFullCentroid(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::FullCentroid);
}
void uiMainWindow::posSubLS(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::SubLS);
}
void uiMainWindow::posTwoCenter(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::TwoCenter);
}
void uiMainWindow::posTaylorSeries(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::Taylor);
}
void uiMainWindow::posWeightedTaylor(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::WeightedTaylor);
}
void uiMainWindow::posKalmanTaylor(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::KalmanTaylor);
}
void uiMainWindow::posCalc_ARM(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::ARM_calcPos);
}

/***********************************************************/
// TRACK
void uiMainWindow::trackCalcPROCESS(TRACK_METHOD type) {
    resetUi(false, true);
    if (TRACK_METHOD::TRACK_KALMAN == type) {
        ui->actionKalmanTrack->setChecked(true);
    } else if (TRACK_METHOD::TRACK_KALMAN_LITE == type) {
        ui->actionKalmanLiteTrack->setChecked(true);
    } else if (TRACK_METHOD::TRACK_KALMAN_INFO == type) {
        ui->actionKalmanInfoTrack->setChecked(true);
    } else {}

    // determine the calculate method
    calcTrack.calcTrackMethod = type;

    QTime time;
    time.start();
    foreach (storeTagInfo *info, store.tags) {
        info->addOrResetMethodInfo(TRACKx_STR, TRACK_METHOD2STR[type]);
/****** CALC TRACK MAIN ****************************************/
        calcTrack.calcOneTrack(info->methodInfo[MEASUR_STR], info->methodInfo[TRACKx_STR]);
/***********************************************************/
        ui->UsrFrm->setUsrStatus(info->tagId, USR_STATUS::HAS_TRACK_DATA);

        dType measDist   = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        dType kalmanDist = calcTotalAvgDistanceSquare(info->methodInfo[TRACKx_STR].AnsLines);
        qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type] << info->toString();
        qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type]
                 << "avgDistanceSquare => measDist:" << measDist << ";"
                 << "trackDist:" << kalmanDist;
    }
    calcTimeElapsedTrack = time.elapsed()/1000.f;
    setStatusTimeInfo();
    qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type]
             << "total Pos:" << totalPos << ";"
             << "using Time:" << calcTimeElapsedTrack << "(s)";

    handleModelDataUpdate(false);
}

void uiMainWindow::trackKalman(bool checked) {
    Q_UNUSED(checked);
    trackCalcPROCESS(TRACK_METHOD::TRACK_KALMAN);
}
void uiMainWindow::trackKalmanLite(bool checked) {
    Q_UNUSED(checked);
    trackCalcPROCESS(TRACK_METHOD::TRACK_KALMAN_LITE);
}
void uiMainWindow::trackKalmanInfo(bool checked) {
    Q_UNUSED(checked);
    trackCalcPROCESS(TRACK_METHOD::TRACK_KALMAN_INFO);
}
