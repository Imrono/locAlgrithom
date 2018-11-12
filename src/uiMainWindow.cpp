#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QFileDialog>
#include "calcTagTrack.h"

uiMainWindow::uiMainWindow(QWidget *parent) :
    QMainWindow(parent), stepShowTimerStarted(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resource/icon/locAlg.png"));
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

    // status bar shows the "calcTimeElapsed"
    calcTimeElapsed = new QLabel(this);
    calcTimeElapsed->setAlignment(Qt::AlignRight);
    calcTimeElapsed->setFrameStyle(QFrame::Box | QFrame::Sunken);
    statusBar()->addPermanentWidget(calcTimeElapsed);

    iterationNum = new QLabel(this);
    statusBar()->addWidget(iterationNum);
    setStatusIter(0, 0.f);

    calcAnalyzeInput[CALC_LOG_ANALYZE] = new dataInput_calcAnalyzeSet(CALC_LOG_ANALYZE, this);
    calcAnalyzeInput[CALC_TEST_ANALYZE] = new dataInput_calcAnalyzeSet(CALC_TEST_ANALYZE, this);
    calcAnalyzeInput[CALC_CAN_ANALYZE] = new dataInput_calcAnalyzeSet(CALC_CAN_ANALYZE, this);
    calcAnalyzeInput[CALC_POS_ANALYZE] = new dataInput_calcAnalyzeSet(CALC_POS_ANALYZE, this);

    analyzeStatus = CALC_LOG_ANALYZE;
    ui->cbAnalyzeMode->addItem(calcAnalyzeInput[CALC_LOG_ANALYZE]->strName);
    ui->cbAnalyzeMode->addItem(calcAnalyzeInput[CALC_TEST_ANALYZE]->strName);
    ui->cbAnalyzeMode->addItem(calcAnalyzeInput[CALC_CAN_ANALYZE]->strName);
    ui->cbAnalyzeMode->addItem(calcAnalyzeInput[CALC_POS_ANALYZE]->strName);
    ui->cbAnalyzeMode->setCurrentIndex(CALC_LOG_ANALYZE);

    connect(&calcAnalyzeInput[CALC_LOG_ANALYZE]->usrFrame, SIGNAL(oneUsrBtnClicked_siganl(int, bool)),
            this, SLOT(oneUsrBtnClicked(int, bool)));
    connect(&calcAnalyzeInput[CALC_LOG_ANALYZE]->usrFrame, SIGNAL(oneUsrShowML_siganl(int, bool)),
            this, SLOT(oneUsrShowML(int, bool)));
    connect(&calcAnalyzeInput[CALC_LOG_ANALYZE]->usrFrame, &uiUsrFrame::oneUsrShowDistance_siganl, this, [this](int tagId) {
        distanceShowTagId = this->calcAnalyzeInput[CALC_LOG_ANALYZE]->usrFrame.isShowable(tagId) ? tagId : UN_INIT_SHOW_TAGID;
        handleModelDataUpdate(false);
    });
	connect(&calcAnalyzeInput[CALC_POS_ANALYZE]->usrFrame, SIGNAL(oneUsrBtnClicked_siganl(int, bool)),
		this, SLOT(oneUsrBtnClicked(int, bool)));

    setStatusDistCount();
    setStatusTimeInfo();

    // load initial CFG and DIST data
#if 1
    loadIniConfigFile(MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/机车配置文件/demo-6d.ini"));
    //loadIniConfigFile(MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/config/WC50Y(B)型支架运输车.ini"));
    loadLogDistanceFile(2, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/太原WC50Y(B)/distance/201705181600.log"));
#else
    loadIniConfigFile(MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/config/石煤测试5.ini"));
    loadLogDistanceFile(1, MY_STR("C:/Users/rono_/Desktop/locationWithKalman/data/石煤测试相关文件/distance/201712201435.log"));
#endif

    // SET NLOS FOR calcPos
    calcNlos = calcPos.getNlosJudge();

    checkData();

    // connect signals and slots for uiMainWindow
    connectUi();

    // initial calculate method
    //nlosRes(true);
    //nlosMultiPoint(true);
    //posWeightedTaylor();
    //trackKalman(true);

    handleModelDataUpdate(false);
}

uiMainWindow::~uiMainWindow()
{
	delete calcAnalyzeInput[CALC_LOG_ANALYZE];
	delete calcAnalyzeInput[CALC_TEST_ANALYZE];
	delete calcAnalyzeInput[CALC_CAN_ANALYZE];
	delete calcAnalyzeInput[CALC_POS_ANALYZE];
    delete ui;
}

void uiMainWindow::oneUsrBtnClicked(int tagId, bool isShowable) {
    Q_UNUSED(tagId);
    ui->canvas->syncWithUiFrame(&getUsrFrame());
    if (!isShowable && distanceShowTagId == tagId) {
        distanceShowTagId = UN_INIT_SHOW_TAGID; // propose to find another tagId to show distance
    }
    handleModelDataUpdate(false);
}
void uiMainWindow::oneUsrShowML(int tagId, bool isShowML) {
    Q_UNUSED(tagId);
    ui->canvas->setShowLM(isShowML);
    handleModelDataUpdate(false);
}
void uiMainWindow::sigmaChanged(int sigma) {
    ui->canvas->setSigmaLM(sigma);
    qDebug() << "[@uiMainWindow::sigmaChanged] sigma:" << sigma;
    if (ui->canvas->getShowLM())
        handleModelDataUpdate(false);
}

// feed the data to canvas->showTagRelated at different time point
void uiMainWindow::handleModelDataUpdate(bool isUpdateCount, bool isShowRT) {
    if (CALC_POS_ANALYZE == analyzeStatus) {
        return;
    }

    uiUsrFrame &usrFrame = getUsrFrame();
    showTagModel &store = getStore();
    int &counting = getCounting();
    if (!isShowRT) {
        counting = counting < 0 ? 0 : counting;
        // distCount updated automaticly when timeout event occurs
        if (isUpdateCount) {
            counting++;
        }
    } else {
        foreach (storeTagInfo* tag, store.tags) {
            if (nullptr == tag) {
                continue;
            } else {
                // get max count
                counting = counting > tag->methodInfo[MEASUR_STR].Ans.count()-1 ?
                            counting : tag->methodInfo[MEASUR_STR].Ans.count()-1;
            }
        }
    }

    // update the status bar info
    setStatusDistCount();

    // clear
    ui->canvas->resetPos();

    bool hasDataShow = false;
    foreach (storeTagInfo* tag, store.tags) {
        if (nullptr == tag) {
            continue;
        }
        int tagID = tag->tagId;

        storeMethodInfo &measInfo = tag->methodInfo[MEASUR_STR];
        // CONDITIONs to the data
        // 1. the user wants to show
        if (usrFrame.isShowable(tagID)
        // 2. MEASURE (position) is sucessful processed
        && tag->isTagPosInitialed
        // 3. data available
        && measInfo.Ans.count() > 0) {
            int tagCounting = 0;
            if (counting < measInfo.Ans.count()) {
                tagCounting = counting;
            } else {
                tagCounting = measInfo.Ans.count() - 1;
            }

            hasDataShow = true;

            calcPosData &posReqData = tag->calcPosReqData[tagCounting];
            // int distanceCount = logData.distance.count();
            int sensorCount = cfgData.get_q()->sensor.count();
            // location pos calc part
            if (CALC_POS_TYPE::POS_NONE != store.calcPosType) {
                ui->canvas->setPosition(tagID, MEASUR_STR, measInfo.Ans[tagCounting].toQPointF());
                ui->canvas->setLine(tagID, MEASUR_STR, measInfo.AnsLines[tagCounting]);
                ui->canvas->setLines(tagID, MEASUR_STR, measInfo.AnsLines);

                usrFrame.setBtnToolTip(tagID, true,
                                       posReqData.distance.data(),
                                       tag->weight[tagCounting].data(),
                                       cfgData.get_q()->sensor.data(),
                                       sensorCount,
                                       measInfo.Ans[tagCounting].toQPointF());
            } else {
                ui->canvas->removeTagMethod(tagID, MEASUR_STR);
                usrFrame.setBtnToolTip(tagID, false);
            }

            ui->canvas->setPointsRaw(tagID, MEASUR_STR, tag->RawPoints[tagCounting]);
            ui->canvas->setPointsRefined(tagID, MEASUR_STR, tag->RefinedPoints[tagCounting]);

            qDebug() << "[@handleModelDataUpdate]" << tagCounting << QString("<%1>").arg(tagID)
                     << measInfo.Ans[tagCounting].toQPointF();

            if (TRACK_METHOD::TRACK_NONE != store.calcTrackMethod) {
                storeMethodInfo &trackInfo = tag->methodInfo[TRACKx_STR];
                ui->canvas->setPosition(tagID, TRACKx_STR, trackInfo.Ans[tagCounting].toQPointF());
                ui->canvas->setLine(tagID, TRACKx_STR, trackInfo.AnsLines[tagCounting]);
                ui->canvas->setLines(tagID, TRACKx_STR, trackInfo.AnsLines);
            } else {
                ui->canvas->removeTagMethod(tagID, TRACKx_STR);
            }

            // intermediate results
            ui->canvas->setDistance(tagID, posReqData.distance, tag->usedSensor[tagCounting]);
            ui->canvas->setWeight(tagID, tag->weight[tagCounting]);
            ui->canvas->setIterPoints(tagID, tag->iterPoints[tagCounting]);

            if (tag->isGaussPointAdded) {    // kalmanCoupled show at LM
                ui->canvas->setGaussPoint(tagID, true, tag->x_hat[tagCounting]);
            } else {
                ui->canvas->setGaussPoint(tagID, false);
            }

            // items in uiMainWindow
            if (calcAnalyzeInput[CALC_TEST_ANALYZE]->isActive
            || (distanceShowTagId == tagID || UN_INIT_SHOW_TAGID == distanceShowTagId)) {
                QPalette pa;
                pa.setColor(QPalette::WindowText, usrFrame.getBtnColorSample(tagID));
                ui->label_Id->setPalette(pa);
                ui->label_Id->setText("<b>"+QString("%0").arg(tagID, 4, 10, QChar('0'))+"<\b>");
                switch (sensorCount) {
                case 6: SHOW_DIST_DATA(5, tag->weight[tagCounting], posReqData.distance);
                case 5: SHOW_DIST_DATA(4, tag->weight[tagCounting], posReqData.distance);
                case 4: SHOW_DIST_DATA(3, tag->weight[tagCounting], posReqData.distance);
                case 3: SHOW_DIST_DATA(2, tag->weight[tagCounting], posReqData.distance);
                case 2: SHOW_DIST_DATA(1, tag->weight[tagCounting], posReqData.distance);
                case 1: SHOW_DIST_DATA(0, tag->weight[tagCounting], posReqData.distance);
                default:
                    break;
                }
                ui->raw_p->setStyleSheet("color:black");
                ui->raw_p->setText("0");
                if (tag->weight[tagCounting].count() > sensorCount) {   // weight[tagCounting][sensorCount] is x^hat weight
                    ui->weight_p->setText(QString::number(tag->weight[tagCounting][sensorCount], 'f', 3));
                }
                if (tag->isGaussPointAdded) {    // kalmanCoupled show at LM
                    dType dist = calcDistance(measInfo.Ans[tagCounting], tag->x_hat[tagCounting]);
                    ui->refine_p->setText(QString::number(dist));
                } else {
                    ui->refine_p->setText("NaN");
                }

                QVector<dType>* ansQuality = measInfo.data;
                setStatusIter(tag->iterPoints[tagCounting].count(),
                              ansQuality[storeMethodInfo::STORED_MSE][tagCounting],
                              ansQuality[storeMethodInfo::STORED_Crossed1][tagCounting] + 0.1f,   // round
                              ansQuality[storeMethodInfo::STORED_Crossed2][tagCounting] + 0.1f);

                usrFrame.setShowDistTagId(tagID);
            }
        } else {
            ui->canvas->clearData(tagID);
        }
    }
    distanceShowTagId = usrFrame.getShowDistTagId();

    update();

    if (!hasDataShow) {
        if (stepShowTimer.isActive()) {
            stepShowTimer.stop();
        }
    }
}
/***********************************************************/
// MENU ACTION
/***********************************************************/
// FILE
void uiMainWindow::loadIniConfigFile(QString pathIn) {
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
    for (int i{0}; i < NUM_ANALYZE_STATUS; i++) {
        calcAnalyzeInput[i]->dataInputHandler->setSensorNum(cfgData.get_q()->sensor.count());
    }

    checkData();
    lastIniPath = path;

    if (isInitKeep && nSensorKeep != cfgData.get_q()->sensor.count()) {
        qDebug() << "[@uiMainWindow::loadIniConfigFile] nSensor changed, need to reload distance file";
        loadLogDistanceFile(2);
    }
}

void uiMainWindow::loadLogDistanceFile(int type, QString pathIn) {
    resetData();

    calcAnalyzeInput[CALC_LOG_ANALYZE]->modelStore.calcPosType = CALC_POS_TYPE::POS_NONE;
    calcAnalyzeInput[CALC_LOG_ANALYZE]->modelStore.calcTrackMethod = TRACK_METHOD::TRACK_NONE;

    QString path;
    if (0 == pathIn.length()) {
        path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    } else {
        path = pathIn;
    }
    if (0 == path.length()) {
        return;
    }
    qDebug() << "[@uiMainWindow::loadLogDistanceFile] type" << type << "Path:" << path;

    dataInputLog &distData = *static_cast<dataInputLog *>(calcAnalyzeInput[CALC_LOG_ANALYZE]->dataInputHandler);
    distData.loadNewFile(type, path);

    // initWithDistanceData
    showTagModel &store = getStore();
    uiUsrFrame *usrFrame = &getUsrFrame();
    foreach (oneTag tag, distData.get_q()->tagsData) {
        store.addNewTagInfo(tag.tagId);
        usrFrame->setBtnEnableLM(tag.tagId, false);
        usrFrame->addOneUsr(tag.tagId, USR_STATUS::HAS_DISTANCE_DATA);
        ui->canvas->setDistance(tag.tagId, tag.distData[0].distance);
    }
    for (int i = 0; i < 32 - distData.get_q()->tagsData.count(); i++) {
        usrFrame->addOneUsr(0, USR_STATUS::HAS_NONE_DATA);
    }

    ui->canvas->syncWithUiFrame(&getUsrFrame());

    ui->actionRead_dist->setChecked(1 == type);
    ui->actionRead_dist_2->setChecked(2 == type);
    qDebug() << "[@uiMainWindow::loadLogDistanceFile]" << ((dataInputLog*)calcAnalyzeInput[CALC_LOG_ANALYZE]->dataInputHandler)->toString();

    checkData();
    lastDistancePath = path;
}

void uiMainWindow::loadPictureFile(QString pathIn) {
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
void uiMainWindow::nlosWylie() {
    ui->actionMultiPoint->setChecked(false);
    if (calcNlos->predictNlos != POINTS_NLOS::WYLIE) {
        ui->actionWylie->setChecked(true);
        calcNlos->predictNlos = POINTS_NLOS::WYLIE;
    } else {
        ui->actionWylie->setChecked(false);
        calcNlos->predictNlos = POINTS_NLOS::POINTS_NONE;
    }
    qDebug() << "[@nlosWylie] clicked";
}

void uiMainWindow::nlosMultiPoint() {
    ui->actionWylie->setChecked(false);
    if (calcNlos->predictNlos != POINTS_NLOS::MULTI_POINT) {
        calcNlos->predictNlos = POINTS_NLOS::MULTI_POINT;
        ui->actionMultiPoint->setChecked(true);
    } else {
        calcNlos->predictNlos = POINTS_NLOS::POINTS_NONE;
        ui->actionMultiPoint->setChecked(false);
    }
    qDebug() << "[@nlosMultiPoint] clicked";
}

void uiMainWindow::nlosRes() {
    ui->actionSumDist->setChecked(false);
    if (calcNlos->precNlos != POS_PRECISION_NLOS::RESIDUAL) {
        calcNlos->precNlos = POS_PRECISION_NLOS::RESIDUAL;
        ui->actionRes->setChecked(true);
    } else {
        calcNlos->precNlos = POS_PRECISION_NLOS::POS_PRECISION_NONE;
        ui->actionRes->setChecked(false);
    }
    qDebug() << "[@nlosRes] clicked";
}

void uiMainWindow::nlosSumDist() {
    ui->actionRes->setChecked(false);
    if (calcNlos->precNlos != POS_PRECISION_NLOS::SUM_DIST) {
        calcNlos->precNlos = POS_PRECISION_NLOS::SUM_DIST;
        ui->actionSumDist->setChecked(true);
    } else {
        calcNlos->precNlos = POS_PRECISION_NLOS::POS_PRECISION_NONE;
        ui->actionSumDist->setChecked(false);
    }
    qDebug() << "[@nlosSumDist] clicked";
}

/***********************************************************/
// POSITION
void uiMainWindow::posCalcPROCESS(CALC_POS_TYPE type, int tagId) {
    showTagModel &store = getStore();

    QTime time;
    time.start();
    store.totalPos = 0;
    foreach (storeTagInfo *info, store.tags) {
        if (0 != tagId) {   // tagId is !0 means real time
            if (info) {
                if (info->tagId != tagId) {
                    continue;
                }
            } else {
                continue;
            }
        }

        ui->canvas->clearData(info->tagId);
        if (CALC_LOG_ANALYZE == analyzeStatus || CALC_TEST_ANALYZE == analyzeStatus) {
            info->clear();
            info->addOrResetMethodInfo(MEASUR_STR, CALC_POS2STR[type]);
        }
        info->calcPosType = type;   // determine the position calculate method
        info->kalmanCoupledType = store.kalmanCoupledType;

        uiUsrFrame *usrFrame = &getUsrFrame();
        // if none position is calculated
        if (CALC_POS_TYPE::POS_NONE != type) {
            info->isTagPosInitialed = true;
            usrFrame->setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);

            QVector<oneLogData> distData;
            bool isUpdateP_t_1 = true;
            if (CALC_LOG_ANALYZE == analyzeStatus) {
                distData = static_cast<dataInputLog *>(calcAnalyzeInput[CALC_LOG_ANALYZE]->dataInputHandler)->get_q()->tagsData[info->tagId].distData;
            } else
            if (CALC_TEST_ANALYZE == analyzeStatus) {
                distData.append(static_cast<dataInputTest *>(calcAnalyzeInput[CALC_TEST_ANALYZE]->dataInputHandler)->oneDistData);
            } else
            if (CALC_CAN_ANALYZE == analyzeStatus) {
                if (0 == tagId) {
                    continue;
                }
                distData.append(static_cast<dataInputCan *>(calcAnalyzeInput[CALC_CAN_ANALYZE]->dataInputHandler)->getDistData());
                isUpdateP_t_1 = false;
            } else
            if (CALC_POS_ANALYZE == analyzeStatus) {
                continue;
            }
/****** CALC POS MAIN BEGIN **************************************************/
            calcPos.calcPosVector(info, distData, isUpdateP_t_1);
/**********************************************************CALC POS MAIN END */
            usrFrame->setBtnEnableLM(info->tagId, true);

            qDebug() << "[@uiMainWindow::posCalcPROCESS]"
                     << info->methodInfo[MEASUR_STR].AnsV.count()
                     << info->methodInfo[MEASUR_STR].AnsA.count();
            usrFrame->setChartData(info->tagId, MEASUR_STR,
                                   info->methodInfo[MEASUR_STR].AnsV,
                                   info->methodInfo[MEASUR_STR].AnsA);
        } else {
            info->isTagPosInitialed = false;
            getUsrFrame().setUsrStatus(info->tagId, USR_STATUS::HAS_DISTANCE_DATA);

            usrFrame->setChartData(info->tagId, MEASUR_STR, QVector<qreal>(), QVector<qreal>());
        }

        // measure pos changed, track-info need re_calc, only clear the it here
        info->reset(TRACKx_STR);
        info->calcTrackMethod = TRACK_METHOD::TRACK_NONE;
        actionNowTrack = nullptr;

        store.totalPos += info->methodInfo[MEASUR_STR].Ans.count();

        dType measDist = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type] << info->toString();
        qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type]
                 << "avgDistanceSquare => measDist:" << measDist
                 << info->methodInfo[MEASUR_STR].AnsLines.count();
    }
    store.calcTimeElapsedMeasu = time.elapsed()/1000.f;
    store.calcTimeElapsedTrack = 0.f;
    setStatusTimeInfo();
    qDebug() << "#posCalcPROCESS#" << CALC_POS2STR[type]
             << "total Pos:" << store.totalPos << ";"
             << "using Time:" << store.calcTimeElapsedMeasu << "(s)";

    ui->actionKalmanTrack->setChecked(false);
    ui->actionKalmanLiteTrack->setChecked(false);
    ui->actionKalmanInfoTrack->setChecked(false);
    store.calcTrackMethod = TRACK_METHOD::TRACK_NONE;

    handleModelDataUpdate(false);
}

void uiMainWindow::posFullCentroid() {
    UPDATE_POS_UI(ui->actionFullCentroid);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::FullCentroid : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posSubLS() {
    UPDATE_POS_UI(ui->actionSubLS);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::SubLS : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posTwoCenter() {
    UPDATE_POS_UI(ui->actionTwoCenter);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::TwoCenter : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posTaylorSeries() {
    UPDATE_POS_UI(ui->actionTaylorSeries);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::Taylor : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posWeightedTaylor() {
    UPDATE_POS_UI(ui->actionWeightedTaylor);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::WeightedTaylor : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
/* kalman coupled methods start **********************************************/
void uiMainWindow::posKalmanCoupled() {
    UPDATE_POS_UI(ui->actionKalmanCoupled);
    kalmanCoupledSyncUi();
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::POS_KalmanCoupled : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posKalmanTrail() {
    getStore().kalmanCoupledType ^= KALMAN_COUPLED_TYPE::TRAIL_COUPLED;
    kalmanCoupledSyncUi();
    getStore().calcPosType = CALC_POS_TYPE::POS_KalmanTrail;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posKalmanGauss() {
    getStore().kalmanCoupledType ^= KALMAN_COUPLED_TYPE::GAUSS_COUPLED;
    kalmanCoupledSyncUi();
    getStore().calcPosType = CALC_POS_TYPE::POS_KalmanGauss;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posKalmanWeight() {
    getStore().kalmanCoupledType ^= KALMAN_COUPLED_TYPE::WEIGHT_COUPLED;
    kalmanCoupledSyncUi();
    getStore().calcPosType = CALC_POS_TYPE::POS_KalmanWeight;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posKalmanSmooth() {
    getStore().kalmanCoupledType ^= KALMAN_COUPLED_TYPE::SMOOTH_COUPLED;
    kalmanCoupledSyncUi();
    getStore().calcPosType = CALC_POS_TYPE::POS_KalmanSmooth;
    posCalcPROCESS(getStore().calcPosType);
}
/************************************************ kalman coupled methods end */
void uiMainWindow::posLMedS() {
    UPDATE_POS_UI(ui->actionLMedS);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::LMedS : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
void uiMainWindow::posBilateration() {
    UPDATE_POS_UI(ui->actionBilateration);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::Bilateration : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}
// ARM VERSION /////////////////////////////////////////////////////////////////
void uiMainWindow::posCalc_ARM() {
    UPDATE_POS_UI(ui->actioncalcTagPos_ARM);
    getStore().calcPosType = actionNowPos ? CALC_POS_TYPE::ARM_calcPos : CALC_POS_TYPE::POS_NONE;
    posCalcPROCESS(getStore().calcPosType);
}

/***********************************************************/
// TRACK
void uiMainWindow::trackCalcPROCESS(TRACK_METHOD type) {
    showTagModel &store = getStore();
    store.calcTrackMethod = type;

    uiUsrFrame *usrFrame = &getUsrFrame();
    // reset kalman parameter of existing tag
    calcTrack.clearParam();
    QTime time;
    time.start();
    foreach (storeTagInfo *info, store.tags) {
        info->addOrResetMethodInfo(TRACKx_STR, TRACK_METHOD2STR[type]);
        info->calcTrackMethod = type;   // determine the track calculate method

        // if none track is calculated
        if (TRACK_METHOD::TRACK_NONE != type) {
            getUsrFrame().setUsrStatus(info->tagId, USR_STATUS::HAS_TRACK_DATA);
/* CALC TRACK MAIN BEGIN *****************************************************/
            calcTrack.calcTrackVector(info->methodInfo[MEASUR_STR], info->methodInfo[TRACKx_STR]);
/********************************************************CALC TRACK MAIN END */

            usrFrame->setChartData(info->tagId, TRACKx_STR,
                                   info->methodInfo[TRACKx_STR].AnsV,
                                   info->methodInfo[TRACKx_STR].AnsA);
        } else {
            getUsrFrame().setUsrStatus(info->tagId, USR_STATUS::HAS_MEASURE_DATA);
            usrFrame->setChartData(info->tagId, MEASUR_STR,
                                   info->methodInfo[MEASUR_STR].AnsV,
                                   info->methodInfo[MEASUR_STR].AnsA);
        }

        dType measDist   = calcTotalAvgDistanceSquare(info->methodInfo[MEASUR_STR].AnsLines);
        dType kalmanDist = calcTotalAvgDistanceSquare(info->methodInfo[TRACKx_STR].AnsLines);
        qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type] << info->toString();
        qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type]
                 << "avgDistanceSquare => measDist:" << measDist << ";"
                 << "trackDist:" << kalmanDist;
    }
    store.calcTimeElapsedTrack = time.elapsed() / 1000.f;
    setStatusTimeInfo();
    qDebug() << "#trackCalcPROCESS#" << TRACK_METHOD2STR[type]
             << "total Pos:" << store.totalPos << ";"
             << "using Time:" << store.calcTimeElapsedTrack << "(s)";

    handleModelDataUpdate(false);
}

void uiMainWindow::trackKalman() {
    UPDATE_TRACK_UI(ui->actionKalmanTrack);
    trackCalcPROCESS(actionNowTrack ? TRACK_METHOD::TRACK_KALMAN : TRACK_METHOD::TRACK_NONE);
}
void uiMainWindow::trackKalmanLite() {
    UPDATE_TRACK_UI(ui->actionKalmanLiteTrack);
    trackCalcPROCESS(actionNowTrack ? TRACK_METHOD::TRACK_KALMAN_LITE : TRACK_METHOD::TRACK_NONE);
}
void uiMainWindow::trackKalmanInfo() {
    UPDATE_TRACK_UI(ui->actionKalmanInfoTrack);
    trackCalcPROCESS(actionNowTrack ? TRACK_METHOD::TRACK_KALMAN_INFO : TRACK_METHOD::TRACK_NONE);
}


void uiMainWindow::on_refresh_clicked()
{
	showTagModel &store = getStore();
    foreach (storeTagInfo *info, store.tags) {
        if (info) {
            ui->canvas->clearData(info->tagId);
            if (CALC_LOG_ANALYZE == analyzeStatus || CALC_TEST_ANALYZE == analyzeStatus) {
                info->clear();
                info->addOrResetMethodInfo(MEASUR_STR, CALC_POS2STR[store.calcPosType]);
            }
        }
    }

    if (actionNowPos) {
        posCalcPROCESS(getStore().calcPosType);
    }

    if (actionNowTrack) {
        trackCalcPROCESS(getStore().calcTrackMethod);
    }
}
