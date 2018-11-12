#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QClipboard>

// kalman coupled position calc related
void uiMainWindow::UPDATE_POS_UI(QAction *action) {
    if (!actionNowPos) {    // 1st time
        actionNowPos = action;
        actionNowPos->setChecked(true);
        if (action != ui->actionKalmanCoupled) {
            kalmanCoupledChange(false);
        } else {
            kalmanCoupledChange(true);
        }
    } else {
        if (actionNowPos != ui->actionKalmanCoupled && action == ui->actionKalmanCoupled) {
            kalmanCoupledChange(true);  // other -> kalmanCoupled
        } else
        if (actionNowPos == ui->actionKalmanCoupled && action != ui->actionKalmanCoupled) {
            kalmanCoupledChange(false); // kalmanCoupled -> other
        } else {}

        if (actionNowPos == action) {   // cancel this calcuation
            actionNowPos->setChecked(false);
            kalmanCoupledChange(false);
            actionNowPos = nullptr;
        } else {                        // change to other calc method
            actionNowPos->setChecked(false);
            actionNowPos = action;
            actionNowPos->setChecked(true);
        }
    }
}

void uiMainWindow::kalmanCoupledChange(bool isEnable) {
    qDebug() << "[@uiMainWindow::kalmanCoupledChange]" << isEnable;
    ui->actionKalmanTrail->setEnabled(isEnable);
    ui->actionKalmanGauss->setEnabled(isEnable);
    ui->actionKalmanWeight->setEnabled(isEnable);
    ui->actionKalmanSmooth->setEnabled(isEnable);
}

void uiMainWindow::kalmanCoupledSyncUi() {
    showTagModel &store = getStore();
    ui->actionKalmanTrail->setChecked (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::TRAIL_COUPLED);
    ui->actionKalmanGauss->setChecked (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::GAUSS_COUPLED);
    ui->actionKalmanWeight->setChecked(store.kalmanCoupledType & KALMAN_COUPLED_TYPE::WEIGHT_COUPLED);
    ui->actionKalmanSmooth->setChecked(store.kalmanCoupledType & KALMAN_COUPLED_TYPE::SMOOTH_COUPLED);
    qDebug() << "[@uiMainWindow::kalmanCoupledSyncUi]" << store.kalmanCoupledType << ";"
             << "TRAIL_COUPLED"  << (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::TRAIL_COUPLED) << ";"
             << "GAUSS_COUPLED"  << (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::GAUSS_COUPLED) << ";"
             << "WEIGHT_COUPLED" << (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::WEIGHT_COUPLED) << ";"
             << "SMOOTH_COUPLED" << (store.kalmanCoupledType & KALMAN_COUPLED_TYPE::SMOOTH_COUPLED) << ";";
}

void uiMainWindow::reflashUI() {
    showTagModel &store = getStore();

    if (CALC_POS_TYPE::POS_NONE != store.calcPosType) {
        posCalcPROCESS(store.calcPosType);
    } else {}
    if (TRACK_METHOD::TRACK_NONE != store.calcTrackMethod) {
        trackCalcPROCESS(store.calcTrackMethod);
    } else {}
}

// track calc related
void uiMainWindow::UPDATE_TRACK_UI(QAction *action) {
    if (actionNowTrack && actionNowTrack == action) {
        actionNowTrack->setChecked(false);
        actionNowTrack = nullptr;
    } else {
        if (actionNowTrack) {
            actionNowTrack->setChecked(false);
        }
        actionNowTrack = action;
        actionNowTrack->setChecked(true);
    }
}

void uiMainWindow::connectUi() {
    connect(ui->canvas, SIGNAL(mouseChange(int,int)), this, SLOT(showMousePos(int, int)));
/*************************************************************/
    // FILE
    connect(ui->actionRead_ini,     SIGNAL(triggered(bool)), this, SLOT(loadIniConfigFile()));
    connect(ui->actionRead_dist,    &QAction::triggered, this, [this](void) {
        loadLogDistanceFile(1);
    });
    connect(ui->actionRead_dist_2,  &QAction::triggered, this, [this](void) {
        loadLogDistanceFile(2);
    });
    connect(ui->actionRead_picture, SIGNAL(triggered(bool)), this, SLOT(loadPictureFile()));

    // NLOS
    connect(ui->actionWylie,     SIGNAL(triggered(bool)), this, SLOT(nlosWylie()));
    connect(ui->actionMultiPoint,SIGNAL(triggered(bool)), this, SLOT(nlosMultiPoint()));
    connect(ui->actionRes,       SIGNAL(triggered(bool)), this, SLOT(nlosRes()));
    connect(ui->actionSumDist,   SIGNAL(triggered(bool)), this, SLOT(nlosSumDist()));

    // POSITION
    connect(ui->actionFullCentroid,    SIGNAL(triggered(bool)), this, SLOT(posFullCentroid()));
    connect(ui->actionSubLS,           SIGNAL(triggered(bool)), this, SLOT(posSubLS()));
    connect(ui->actionTwoCenter,       SIGNAL(triggered(bool)), this, SLOT(posTwoCenter()));
    connect(ui->actionTaylorSeries,    SIGNAL(triggered(bool)), this, SLOT(posTaylorSeries()));

    connect(ui->actionWeightedTaylor,  SIGNAL(triggered(bool)), this, SLOT(posWeightedTaylor()));

    connect(ui->actionKalmanCoupled,   SIGNAL(triggered(bool)), this, SLOT(posKalmanCoupled()));
    connect(ui->actionKalmanTrail,     SIGNAL(triggered(bool)), this, SLOT(posKalmanTrail()));
    connect(ui->actionKalmanGauss,     SIGNAL(triggered(bool)), this, SLOT(posKalmanGauss()));
    connect(ui->actionKalmanWeight,    SIGNAL(triggered(bool)), this, SLOT(posKalmanWeight()));
    connect(ui->actionKalmanSmooth,    SIGNAL(triggered(bool)), this, SLOT(posKalmanSmooth()));

    connect(ui->actionLMedS,           SIGNAL(triggered(bool)), this, SLOT(posLMedS()));
    connect(ui->actionBilateration,    SIGNAL(triggered(bool)), this, SLOT(posBilateration()));

    // TRACK
    connect(ui->actionKalmanTrack,     SIGNAL(triggered(bool)), this, SLOT(trackKalman()));
    connect(ui->actionKalmanLiteTrack, SIGNAL(triggered(bool)), this, SLOT(trackKalmanLite()));
    connect(ui->actionKalmanInfoTrack, SIGNAL(triggered(bool)), this, SLOT(trackKalmanInfo()));

    // ZOOM
    connect(ui->actionZoomIn,  SIGNAL(triggered(bool)), this, SLOT(zoomIn(bool)));
    connect(ui->actionZoomOut, SIGNAL(triggered(bool)), this, SLOT(zoomOut(bool)));

    // CANVAS CAPTURE PICTURE
    connect(ui->actionCapPix, SIGNAL(triggered(bool)), this, SLOT(captureCanvas(bool)));

    // ARM
    connect(ui->actioncalcTagPos_ARM, SIGNAL(triggered(bool)), this, SLOT(posCalc_ARM()));
/*****************************************************************************/
    connect(&stepShowTimer, SIGNAL(timeout()), this, SLOT(handleModelDataUpdate()));

    connect(ui->beginTrack, &QPushButton::clicked, this, [this](void) {
        if (stepShowTimerStarted) {
            stepShowTimer.stop();
            this->ui->beginTrack->setText("track");
        } else {
            stepShowTimer.start(500);
            ui->beginTrack->setText("stop");
        }
        stepShowTimerStarted = !stepShowTimerStarted;
    });
    connect(ui->showPath, &QPushButton::clicked, this, [this](void) {
        ui->showPath->setStyleSheet("font-weight: bold;");
        if (!ui->canvas->reverseShowPath()) {
            ui->showPath->setText("PATH");
        } else {
            ui->showPath->setText("!PATH");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->reset, &QPushButton::clicked, this, [this](void) {
        getCounting() = 0;
        handleModelDataUpdate(false);
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        getCounting() --;
        handleModelDataUpdate(false);
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        getCounting() ++;
        handleModelDataUpdate(false);
    });

    connect(ui->showCross, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowCross()) {
            ui->showCross->setText("CROSS");
        } else {
            ui->showCross->setText("!CROSS");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->allPos, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowAllPos()) {
            ui->allPos->setText("Show raw");
        } else {
            ui->allPos->setText("Hide raw");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->showRadius, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowRadius()) {
            ui->showRadius->setText("Radius");
        } else {
            ui->showRadius->setText("!Radius");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->showTrace, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowTrack()) {
            ui->showTrace->setText("iterPath");
        } else {
            ui->showTrace->setText("!iterPath");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->btn_drawTagId, &QPushButton::clicked, this, [this](void) {
        ui->canvas->reverseShowTagId();
        handleModelDataUpdate(false);
    });

    connect(ui->distCountEdit, &QLineEdit::returnPressed, this, [this](void) {
        getCounting() = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
        ui->next->setFocus();
    });
    connect(ui->gotoCount, &QPushButton::clicked, this, [this](void) {
        getCounting() = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
    });

    // set Max Likehood's σ used in showTagRelated
    int sigmaInitValue = 250;
    ui->sigmaSlider->setValue(sigmaInitValue);
    sigmaChanged(sigmaInitValue);
    connect(ui->sigmaSlider, SIGNAL(valueChanged(int)), this, SLOT(sigmaChanged(int)));

    connect(ui->refresh, SIGNAL(clicked()), this, SLOT(reflashUI()));

    // distance show
    connect(ui->raw_0, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
    connect(ui->raw_1, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
    connect(ui->raw_2, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
    connect(ui->raw_3, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
    connect(ui->raw_4, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
    connect(ui->raw_5, &QLineEdit::textChanged, this, [this](void) {
        syncTestDistanceData();
    });
}

void uiMainWindow::syncTestDistanceData() {
    if (calcAnalyzeInput[CALC_TEST_ANALYZE]->isActive) {
        dataInputTest *dataInput = static_cast<dataInputTest *>(calcAnalyzeInput[CALC_TEST_ANALYZE]->dataInputHandler);
        if (dataInput->oneDistData.distance.count() < 6) {
            dataInput->oneDistData.distance = QVector<int>(6, 0);
        }
        dataInput->oneDistData.distance[0] = ui->raw_0->text().toInt();
        dataInput->oneDistData.distance[1] = ui->raw_1->text().toInt();
        dataInput->oneDistData.distance[2] = ui->raw_2->text().toInt();
        dataInput->oneDistData.distance[3] = ui->raw_3->text().toInt();
        dataInput->oneDistData.distance[4] = ui->raw_4->text().toInt();
        dataInput->oneDistData.distance[5] = ui->raw_5->text().toInt();
        ui->canvas->setDistance(TEST_TAG_ID, dataInput->oneDistData.distance);
    }
}

void uiMainWindow::wheelEvent(QWheelEvent *e)
{
    if (QApplication::keyboardModifiers () == Qt::ControlModifier) {
        if(e->delta() > 0) {
            zoomIn(); //放大
        } else {
            zoomOut();//缩小
        }
    } else if (QApplication::keyboardModifiers () == Qt::AltModifier) {
        if(e->delta() > 0) {
            emit ui->previous->clicked();
        } else {
            emit ui->next->clicked();
        }
    } else {}
}

void uiMainWindow::checkData() {
    bool logInited = static_cast<dataInputLog*>(calcAnalyzeInput[analyzeStatus]->dataInputHandler)->get_q()->isInitialized;
    qDebug() << "[@uiMainWindow::checkData()]"
             << "cfgData.get_q() =" << cfgData.get_q()->isInitialized
             << "distData.get_q() =" << logInited;

    bool isEnabled = cfgData.get_q()->isInitialized;
    if (isEnabled) {
        if (analyzeStatus == CALC_LOG_ANALYZE) {
            isEnabled = isEnabled && logInited;
        }
    }

    ui->actionWylie->setEnabled(isEnabled);
    ui->actionMultiPoint->setEnabled(isEnabled);
    ui->actionRes->setEnabled(isEnabled);
    ui->actionSumDist->setEnabled(isEnabled);

    ui->actionFullCentroid->setEnabled(isEnabled);
    ui->actionSubLS->setEnabled(isEnabled);
    ui->actionTwoCenter->setEnabled(isEnabled);
    ui->actionTaylorSeries->setEnabled(isEnabled);

    ui->actionWeightedTaylor->setEnabled(isEnabled);
    ui->actionKalmanCoupled->setEnabled(isEnabled);

    ui->actionLMedS->setEnabled(isEnabled);
    ui->actionBilateration->setEnabled(isEnabled);

    ui->actioncalcTagPos_ARM->setEnabled(isEnabled);

    ui->actionKalmanTrack->setEnabled(isEnabled);
    ui->actionKalmanLiteTrack->setEnabled(isEnabled);
    ui->actionKalmanInfoTrack->setEnabled(isEnabled);
}
void uiMainWindow::resetData() {
    // CLEAR LEGACY DATA
    calcAnalyzeInput[CALC_LOG_ANALYZE]->modelStore.removeAll();
    //static_cast<dataInputLog *>(calcAnalyzeInput[CALC_LOG_ANALYZE]->dataInputHandler)->clear();
    calcAnalyzeInput[CALC_TEST_ANALYZE]->modelStore.removeAll();

    for (int i = 0; i < NUM_ANALYZE_STATUS; i++) {
        calcAnalyzeInput[i]->usrFrame.removeAll();
        calcAnalyzeInput[i]->modelStore.calcPosType = CALC_POS_TYPE::POS_NONE;
        calcAnalyzeInput[i]->modelStore.calcTrackMethod = TRACK_METHOD::TRACK_NONE;
    }
    ui->canvas->removeAll();
    // CLEAR UI
    resetUi(true, true);
}

void uiMainWindow::resetUi(bool isPos, bool isTrack) {
    if (isPos) {
        ui->actionFullCentroid->setChecked(false);
        ui->actionSubLS->setChecked(false);
        ui->actionTwoCenter->setChecked(false);
        ui->actionTaylorSeries->setChecked(false);

        ui->actionWeightedTaylor->setChecked(false);

        ui->actionKalmanCoupled->setChecked(false);

        ui->actionKalmanTrail->setChecked(false);
        ui->actionKalmanGauss->setChecked(false);
        ui->actionKalmanWeight->setChecked(false);
        ui->actionKalmanSmooth->setChecked(false);

        ui->actionLMedS->setChecked(false);
        ui->actionBilateration->setChecked(false);

        ui->actioncalcTagPos_ARM->setChecked(false);
    } else {}

    if (isTrack) {
        ui->actionKalmanTrack->setChecked(false);
        ui->actionKalmanLiteTrack->setChecked(false);
        ui->actionKalmanInfoTrack->setChecked(false);
    } else {}
}

void uiMainWindow::setStatusTimeInfo(bool isCalcPosAnalyze) {
    if (isCalcPosAnalyze) {
        calcTimeElapsed->setText("PLEASE SET PC's IP: <b>192.168.200.205</b> (matches IP1)");
    } else {
        QString posStr;
        QString trackStr;

        showTagModel &store = getStore();
        posStr = CALC_POS2STR[store.calcPosType];
        trackStr = TRACK_METHOD2STR[getStore().calcTrackMethod];

        calcTimeElapsed->setText(QString("<b>nPOS:%0</b> | <b>%1</b>:%2(s) | <b>%3</b>:%4(s)")
                                 .arg(store.totalPos)
                                 .arg(posStr)
                                 .arg(QString::number(store.calcTimeElapsedMeasu, 'g', 4))
                                 .arg(trackStr)
                                 .arg(QString::number(store.calcTimeElapsedTrack)));
    }
}
void uiMainWindow::setStatusDistCount() {
    distCountShow->setText(QString("distCount: <b>%0</b>").arg(getCounting(), 4, 10, QChar('0')));
}
void uiMainWindow::setStatusZoom() {
    distZoomShow->setText(QString("[%0%]")
                          .arg(ui->canvas->zoomGet(), 3, 10, QChar('0')));
}
void uiMainWindow::setStatusMousePos(int x, int y) {
    canvasPosShow->setText(QString("(%0,%1)").arg(x, 4, 10, QChar('0')).arg(y, 4, 10, QChar('0')));
}

void uiMainWindow::zoomIn(bool) {
    ui->canvas->zoomChange(20);
    setStatusZoom();
}
void uiMainWindow::zoomOut(bool) {
    ui->canvas->zoomChange(-20);
    setStatusZoom();
}
void uiMainWindow::captureCanvas(bool) {
    QPixmap pix = QPixmap::grabWidget(ui->canvas);
    QApplication::clipboard()->setPixmap(pix);
}

void uiMainWindow::showMousePos(int x, int y) {
    setStatusMousePos(x, y);

    int ansTagId;
    int ansCount;
    QPointF ansPos;
    bool ansFound{false};

    dType minDistance{25.f};    // if dist < minDistance: ansFound = true, then find the min
    showTagModel &store = getStore();
    uiUsrFrame &usrFrame = getUsrFrame();
    foreach (const storeTagInfo *oneTagInfo, store.tags) {
        if (!oneTagInfo->methodInfo.contains(MEASUR_STR)) {
            continue;
        }

        const storeMethodInfo &measInfo = oneTagInfo->methodInfo[MEASUR_STR];

        if (usrFrame.isShowable(oneTagInfo->tagId) // 1. the user wants to show
        && oneTagInfo->isTagPosInitialed) {         // 2. MEASURE (position) is sucessful processed
            for(int i{0}; i < measInfo.Ans.count(); i++) {
                dType currDist = calcDistance(measInfo.Ans[i].toQPointF(), QPointF(x, y));
                if (minDistance > currDist) {
                    ansTagId = oneTagInfo->tagId;
                    ansCount = i;
                    ansPos = measInfo.Ans[i].toQPointF();
                    minDistance = currDist;
                    ansFound = true;
                }
            }
        }
    }

    ui->canvas->drawClosestPos(ansFound, ansTagId, ansCount, ansPos);
}

void uiMainWindow::setStatusIter(int n, dType mse, int crossed1, int crossed2) {
    iterationNum->setText(QString("nIter:") + QString::number(n) +
                          QString("<") + QString::number(mse) + QString(">") +
                          QString("|r1:") + QString::number(MACRO_circleR_1) +
                          QString("<") + QString::number(crossed1) + QString(">") +
                          QString("|r2:") + QString::number(MACRO_circleR_2) +
                          QString("<") + QString::number(crossed2) + QString(">"));
}

void uiMainWindow::on_cbAnalyzeMode_currentIndexChanged(int index)
{
    qDebug() << "cbAnalyzeMode from" << analyzeStatus << calcAnalyzeInput[analyzeStatus]->strName
                             << "to" << index         << calcAnalyzeInput[index]->strName;

/* reset the enviroment *******************************************************/
    ui->canvas->removeAll();

    if (ui->canvas->getShowPath()) {
        emit ui->showPath->click();
    }
    if (ui->canvas->getShowAllPos()) {
        emit ui->allPos->click();
    }
    if (ui->canvas->getShowRadius()) {
        emit ui->showRadius->click();
    }
    if (ui->canvas->getShowTrack()) {
        emit ui->showTrace->click();
    }
    if (ui->canvas->getShowCross()) {
        emit ui->showCross->click();
    }
    if (ui->canvas->getShowTagId()) {
        emit ui->btn_drawTagId->click();
    }

    if (actionNowPos) {
        actionNowPos->setChecked(false);
        kalmanCoupledChange(false);
        actionNowPos = nullptr;
    }
    if (actionNowTrack) {
        actionNowTrack->setChecked(false);
        actionNowTrack = nullptr;
    }

    showTagModel &store = getStore();
    foreach (storeTagInfo* tag, store.tags) {
        ui->canvas->clearData(tag->tagId);
    }

    resetUi(true, true);
/******************************************************* reset the enviroment */
    calcAnalyzeInput[CALC_LOG_ANALYZE ]->handleModelChange(false, this);
    calcAnalyzeInput[CALC_TEST_ANALYZE]->handleModelChange(false, this);
    calcAnalyzeInput[CALC_CAN_ANALYZE ]->handleModelChange(false, this);
    calcAnalyzeInput[CALC_POS_ANALYZE ]->handleModelChange(false, this);
    distanceShowTagId = UN_INIT_SHOW_TAGID;

    if (CALC_LOG_ANALYZE == index) {
        workingStore = &calcAnalyzeInput[CALC_LOG_ANALYZE]->modelStore;
        workingUsrFrame = &calcAnalyzeInput[CALC_LOG_ANALYZE]->usrFrame;
        calcAnalyzeInput[CALC_LOG_ANALYZE]->handleModelChange(true, this);
    } else
    if (CALC_TEST_ANALYZE == index) {
        workingStore = &calcAnalyzeInput[CALC_TEST_ANALYZE]->modelStore;
        workingUsrFrame = &calcAnalyzeInput[CALC_TEST_ANALYZE]->usrFrame;
        calcAnalyzeInput[CALC_TEST_ANALYZE]->handleModelChange(true, this);
	} else
    if (CALC_CAN_ANALYZE == index) {
        workingStore = &calcAnalyzeInput[CALC_CAN_ANALYZE]->modelStore;
        workingUsrFrame = &calcAnalyzeInput[CALC_CAN_ANALYZE]->usrFrame;
        calcAnalyzeInput[CALC_CAN_ANALYZE]->handleModelChange(true, this);
    } else
    if (CALC_POS_ANALYZE == index) {
        workingStore = &calcAnalyzeInput[CALC_POS_ANALYZE]->modelStore;
        workingUsrFrame = &calcAnalyzeInput[CALC_POS_ANALYZE]->usrFrame;
        calcAnalyzeInput[CALC_POS_ANALYZE]->handleModelChange(true, this);
    } else {}

    analyzeStatus = (ANALYZE_STATUS)index;
}
