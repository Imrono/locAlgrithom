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
    setWindowIcon(QIcon("../resource/icon/locAlg.png"));
    ui->previous->setToolTip(MY_STR("←"));
    ui->next->setToolTip(MY_STR("→"));

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

    // load initial CFG and DIST data
#if 1
    loadIniConfigFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/config/WC50Y(B)型支架运输车.ini"));
    loadLogDistanceFile_2(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/distance/201705181600.log"));
#else
    loadIniConfigFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/config/石煤测试5.ini"));
    loadLogDistanceFile(true, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/distance/201712201435.log"));
#endif

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
void uiMainWindow::oneUsrShowML(int tagId, bool isShowML) {
    Q_UNUSED(tagId);
    ui->canvas->setShowLM(isShowML);
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
            qDebug() << "[@handleModelDataUpdate]" << "<" << tag.tagId << ">" << distCount
                     << oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount].toQPointF();

            if (TRACK_METHOD::TRACK_NONE != calcTrack.calcTrackMethod) {
                ui->canvas->setPosition(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].Ans[distCount].toQPointF());
                ui->canvas->setLine(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].AnsLines[distCount-1]);
            } else {}

            ui->canvas->setPointsRaw(tag.tagId, MEASUR_STR, oneTagInfo->RawPoints[distCount]);
            ui->canvas->setPointsRefined(tag.tagId, MEASUR_STR, oneTagInfo->RefinedPoints[distCount]);

            ui->canvas->setDistance(tag.tagId,
                                    distData.get_q()->tagsData[tag.tagId].distData[distCount].distance.data(),
                                    oneTagInfo->usedSeneor[distCount].data());
            ui->canvas->setWeight(tag.tagId, oneTagInfo->weight[distCount]);
            if (oneTagInfo->isGaussPointAdded) {
                ui->canvas->setGaussPoint(tag.tagId, true, oneTagInfo->x_hat[distCount]);
            } else {
                ui->canvas->setGaussPoint(tag.tagId, false);
            }

            ui->canvas->setIterPoints(tag.tagId, oneTagInfo->iterPoints[distCount]);
            setStatusIter(oneTagInfo->iterPoints[distCount].count(),
                          oneTagInfo->methodInfo[MEASUR_STR].data[0][distCount]);

            ui->canvas->setLines(tag.tagId, MEASUR_STR, oneTagInfo->methodInfo[MEASUR_STR].AnsLines);
            ui->canvas->setLines(tag.tagId, TRACKx_STR, oneTagInfo->methodInfo[TRACKx_STR].AnsLines);

            switch (tag.distData[distCount].distance.count()) {
            case 6:
                SHOW_DIST_WEIGHT(5);
                SHOW_DIST_DIFF(5);
            case 5:
                SHOW_DIST_WEIGHT(4);
                SHOW_DIST_DIFF(4);
            case 4:
                SHOW_DIST_WEIGHT(3);
                SHOW_DIST_DIFF(3);
            case 3:
                SHOW_DIST_WEIGHT(2);
                SHOW_DIST_DIFF(2);
            case 2:
                SHOW_DIST_WEIGHT(1);
                SHOW_DIST_DIFF(1);
            case 1:
                SHOW_DIST_WEIGHT(0);
                SHOW_DIST_DIFF(0);
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
    if (0 == path.length()) {
        return;
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
    if (0 == path.length()) {
        return;
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
    if (0 == path.length()) {
        return;
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
    resetUi(true, false);   // reset pos checked, not track
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
    } else if (CALC_POS_TYPE::POS_KalmanCoupled == type) {
        if (calcPos.kalmanCoupledType & calcTagPos::COUPLED) {
            calcPos.kalmanCoupledType = calcTagPos::NONE_COUPLED;
            ui->actionKalmanCoupled->setChecked(false);
        } else {
            calcPos.kalmanCoupledType = calcTagPos::COUPLED;
            ui->actionKalmanCoupled->setChecked(true);
        }
    } else if (CALC_POS_TYPE::POS_KalmanGauss == type) {
        if (calcPos.kalmanCoupledType & calcTagPos::COUPLED) {
            ui->actionKalmanCoupled->setChecked(true);
            if (calcPos.kalmanCoupledType & calcTagPos::GAUSS_COUPLED) {
                calcPos.kalmanCoupledType &= ~calcTagPos::GAUSS_COUPLED;
                ui->actionKalmanGauss->setChecked(false);
            } else {
                calcPos.kalmanCoupledType |= calcTagPos::GAUSS_COUPLED;
                ui->actionKalmanGauss->setChecked(true);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::WEIGHT_COUPLED) {
                ui->actionKalmanWeight->setChecked(true);
            } else {
                ui->actionKalmanWeight->setChecked(false);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::SMOOTH_COUPLED) {
                ui->actionKalmanSmooth->setChecked(true);
            } else {
                ui->actionKalmanSmooth->setChecked(false);
            }
        } else {
            calcPos.kalmanCoupledType = calcTagPos::NONE_COUPLED;
            ui->actionKalmanGauss->setChecked(false);
        }
    } else if (CALC_POS_TYPE::POS_KalmanWeight == type) {
        if (calcPos.kalmanCoupledType & calcTagPos::COUPLED) {
            ui->actionKalmanCoupled->setChecked(true);
            if (calcPos.kalmanCoupledType & calcTagPos::GAUSS_COUPLED) {
                ui->actionKalmanGauss->setChecked(true);
            } else {
                ui->actionKalmanGauss->setChecked(false);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::WEIGHT_COUPLED) {
                calcPos.kalmanCoupledType &= ~calcTagPos::WEIGHT_COUPLED;
                ui->actionKalmanWeight->setChecked(false);
            } else {
                calcPos.kalmanCoupledType |= calcTagPos::WEIGHT_COUPLED;
                ui->actionKalmanWeight->setChecked(true);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::SMOOTH_COUPLED) {
                ui->actionKalmanSmooth->setChecked(true);
            } else {
                ui->actionKalmanSmooth->setChecked(false);
            }
        } else {
            calcPos.kalmanCoupledType = calcTagPos::NONE_COUPLED;
            ui->actionKalmanGauss->setChecked(false);
        }
    } else if (CALC_POS_TYPE::POS_KalmanSmooth == type) {
        if (calcPos.kalmanCoupledType & calcTagPos::COUPLED) {
            ui->actionKalmanCoupled->setChecked(true);
            if (calcPos.kalmanCoupledType & calcTagPos::GAUSS_COUPLED) {
                ui->actionKalmanGauss->setChecked(true);
            } else {
                ui->actionKalmanGauss->setChecked(false);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::WEIGHT_COUPLED) {
                ui->actionKalmanWeight->setChecked(true);
            } else {
                ui->actionKalmanWeight->setChecked(false);
            }
            if (calcPos.kalmanCoupledType & calcTagPos::SMOOTH_COUPLED) {
                calcPos.kalmanCoupledType &= ~calcTagPos::SMOOTH_COUPLED;
                ui->actionKalmanSmooth->setChecked(false);
            } else {
                calcPos.kalmanCoupledType |= calcTagPos::SMOOTH_COUPLED;
                ui->actionKalmanSmooth->setChecked(true);
            }
        } else {
            calcPos.kalmanCoupledType = calcTagPos::NONE_COUPLED;
            ui->actionKalmanGauss->setChecked(false);
        }
    } else if (CALC_POS_TYPE::LMedS == type) {
        ui->actionLMedS->setChecked(true);
    } else if (CALC_POS_TYPE::Bilateration == type) {
        ui->actionBilateration->setChecked(true);
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
/* kalman coupled methods ****************************************************/
void uiMainWindow::posKalmanCoupled(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::POS_KalmanCoupled);
}
void uiMainWindow::posKalmanGauss(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::POS_KalmanGauss);
}
void uiMainWindow::posKalmanWeight(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::POS_KalmanWeight);
}
void uiMainWindow::posKalmanSmooth(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::POS_KalmanSmooth);
}
/*****************************************************************************/
void uiMainWindow::posLMedS(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::LMedS);
}
void uiMainWindow::posBilateration(bool checked) {
    Q_UNUSED(checked);
    posCalcPROCESS(CALC_POS_TYPE::Bilateration);
}
// ARM VERSION /////////////////////////////////////////////////////////////////
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
    // reset kalman parameter of existing tag
    calcTrack.clearParam();
    QTime time;
    time.start();
    foreach (storeTagInfo *info, store.tags) {
        info->addOrResetMethodInfo(TRACKx_STR, TRACK_METHOD2STR[type]);
/****** CALC TRACK MAIN ****************************************/
        calcTrack.calcTrackVector(info->methodInfo[MEASUR_STR], info->methodInfo[TRACKx_STR]);
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
