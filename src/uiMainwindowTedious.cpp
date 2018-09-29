#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QClipboard>

// kalman coupled position calc related
void uiMainWindow::UPDATE_POS_UI(QAction *action) {
    if (!getActionNowPos()) {    // 1st time
        getActionNowPos() = action;
        getActionNowPos()->setChecked(true);
        if (action != ui->actionKalmanCoupled) {
            kalmanCoupledChange(false);
        } else {
            kalmanCoupledChange(true);
        }
    } else {
        if (getActionNowPos() != ui->actionKalmanCoupled && action == ui->actionKalmanCoupled) {
            kalmanCoupledChange(true);  // other -> kalmanCoupled
        } else if (getActionNowPos() == ui->actionKalmanCoupled && action != ui->actionKalmanCoupled) {
            kalmanCoupledChange(false); // kalmanCoupled -> other
        } else {}

        if (getActionNowPos() == action) {   // cancel this calcuation
            getActionNowPos()->setChecked(false);
            kalmanCoupledChange(false);
            getActionNowPos() = nullptr;
        } else {                        // change to other calc method
            getActionNowPos()->setChecked(false);
            getActionNowPos() = action;
            getActionNowPos()->setChecked(true);
        }
    }
}
void uiMainWindow::kalmanCoupledChange(bool isEnable) {
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
    connect(ui->isTest, SIGNAL(clicked(bool)), this, SLOT(modelChange(bool)));
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
    connect(&timer, SIGNAL(timeout()), this, SLOT(handleModelDataUpdate()));

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
            ui->showPath->setText("显示路径");
        } else {
            ui->showPath->setText("隐藏路径");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->reset, &QPushButton::clicked, this, [this](void) {
        realCounting = 1;
        handleModelDataUpdate(false);
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        realCounting --;
        handleModelDataUpdate(false);
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        realCounting ++;
        handleModelDataUpdate(false);
    });

    connect(ui->showCross, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowCross()) {
            ui->showCross->setText("显示交点");
        } else {
            ui->showCross->setText("隐藏交点");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->allPos, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowAllPos()) {
            ui->allPos->setText("显示raw");
        } else {
            ui->allPos->setText("隐藏raw");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->showRadius, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowRadius()) {
            ui->showRadius->setText("显示半径");
        } else {
            ui->showRadius->setText("隐藏半径");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->showTrace, &QPushButton::clicked, this, [this](void) {
        if (!ui->canvas->reverseShowTrack()) {
            ui->showTrace->setText("显示iter");
        } else {
            ui->showTrace->setText("隐藏iter");
        }
        handleModelDataUpdate(false);
    });
    connect(ui->btn_drawTagId, &QPushButton::clicked, this, [this](void) {
        ui->canvas->reverseShowTagId();
        handleModelDataUpdate(false);
    });

    connect(ui->distCountEdit, &QLineEdit::returnPressed, this, [this](void) {
        realCounting = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
        ui->next->setFocus();
    });
    connect(ui->gotoCount, &QPushButton::clicked, this, [this](void) {
        realCounting = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
    });

    connect(&realUsrFrame, SIGNAL(oneUsrBtnClicked_siganl(int, bool)),
            this, SLOT(oneUsrBtnClicked(int, bool)));
    connect(&realUsrFrame, SIGNAL(oneUsrShowML_siganl(int, bool)),
            this, SLOT(oneUsrShowML(int, bool)));
    connect(&realUsrFrame, &uiUsrFrame::oneUsrShowDistance_siganl, this, [this](int tagId) {
        distanceShowTagId = getUsrFrame().isShowable(tagId) ? tagId : UN_INIT_SHOW_TAGID;
        handleModelDataUpdate(false);
    });
    connect(&fakeUsrFrame, SIGNAL(oneUsrBtnClicked_siganl(int, bool)),
            this, SLOT(oneUsrBtnClicked(int, bool)));
    connect(&fakeUsrFrame, SIGNAL(oneUsrShowML_siganl(int, bool)),
            this, SLOT(oneUsrShowML(int, bool)));
    connect(&fakeUsrFrame, &uiUsrFrame::oneUsrShowDistance_siganl, this, [this](int tagId) {
        distanceShowTagId = getUsrFrame().isShowable(tagId) ? tagId : UN_INIT_SHOW_TAGID;
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
        SET_DISTANCE(0);
    });
    connect(ui->raw_1, &QLineEdit::textChanged, this, [this](void) {
        SET_DISTANCE(1);
    });
    connect(ui->raw_2, &QLineEdit::textChanged, this, [this](void) {
        SET_DISTANCE(2);
    });
    connect(ui->raw_3, &QLineEdit::textChanged, this, [this](void) {
        SET_DISTANCE(3);
    });
    connect(ui->raw_4, &QLineEdit::textChanged, this, [this](void) {
        SET_DISTANCE(4);
    });
    connect(ui->raw_5, &QLineEdit::textChanged, this, [this](void) {
        SET_DISTANCE(5);
    });
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
    qDebug() << "[@uiMainWindow::checkData()]"
             << "cfgData.get_q() =" << cfgData.get_q()->isInitialized
             << "distData.get_q() =" << realDistData.get_q()->isInitialized;
    bool isEnabled = cfgData.get_q()->isInitialized && realDistData.get_q()->isInitialized;
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
    ui->actionKalmanTrail->setEnabled(isEnabled);
    ui->actionKalmanGauss->setEnabled(isEnabled);
    ui->actionKalmanWeight->setEnabled(isEnabled);
    ui->actionKalmanSmooth->setEnabled(isEnabled);

    ui->actionLMedS->setEnabled(isEnabled);
    ui->actionBilateration->setEnabled(isEnabled);

    ui->actioncalcTagPos_ARM->setEnabled(isEnabled);

    ui->actionKalmanTrack->setEnabled(isEnabled);
    ui->actionKalmanLiteTrack->setEnabled(isEnabled);
    ui->actionKalmanInfoTrack->setEnabled(isEnabled);
}
void uiMainWindow::resetData() {
    // CLEAR LEGACY DATA
    realStore.removeAll();
    realDistData.clear();
    realUsrFrame.removeAll();
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

void uiMainWindow::setStatusTimeInfo() {
    QString posStr;
    QString trackStr;
    posStr = CALC_POS2STR[getStore().calcPosType];
    trackStr = TRACK_METHOD2STR[getStore().calcTrackMethod];

    calcTimeElapsed->setText(QString("<b>nPOS:%0</b> | <b>%1</b>:%2(s) | <b>%3</b>:%4(s)")
                             .arg(!isFaked ? totalPos : 1)
                             .arg(posStr)
                             .arg(!isFaked ? QString::number(calcTimeElapsedMeasu) : "NaN")
                             .arg(trackStr)
                             .arg(!isFaked ? QString::number(calcTimeElapsedTrack) : "NaN"));
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

    uiUsrFrame *usrFrame = &getUsrFrame();
    if (&fakeUsrFrame == usrFrame) {    // return if in test model
        return;
    }

    int ansTagId;
    int ansCount;
    QPointF ansPos;
    bool ansFound{false};

    dType minDistance{25.f};    // if dist < minDistance: ansFound = true, then find the min
    showTagModel &store = getStore();
    foreach (const storeTagInfo *oneTagInfo, store.tags) {
        if (!oneTagInfo->methodInfo.contains(MEASUR_STR)) {
            continue;
        }

        const storeMethodInfo &measInfo = oneTagInfo->methodInfo[MEASUR_STR];

        if (usrFrame->isShowable(oneTagInfo->tagId) // 1. the user wants to show
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

void uiMainWindow::modelChange(bool isTest) {
    qDebug() << "isChecked (is test model)" << isTest << "isFaked" << isFaked;
    if (isFaked != isTest) {
        isFaked = isTest;

        //initWithDistanceData();

        ui->actionKalmanInfoTrack->setEnabled(!isFaked);
        ui->actionKalmanLiteTrack->setEnabled(!isFaked);
        ui->actionKalmanTrack->setEnabled(!isFaked);
        //ui->UsrFrm->setEnabledAll(!isFaked);
        ui->canvas->isTestModel = isFaked;

        dataDistanceLog &distData = getDistData();
        showTagModel &store = getStore();
        uiUsrFrame *usrFrame = &getUsrFrame();
        int counting = getCounting();
        distanceShowTagId = usrFrame->getShowDistTagId();   //change showDist data

        const QVector<int> &distance = distData.get_q()->
                tagsData[distanceShowTagId].distData[counting].distance;
        QVector<dType> weight;
        bool isWeighted = true;
        if (UN_INIT_SHOW_TAGID == distanceShowTagId) {
            isWeighted = false;
        } else if (store.tags[distanceShowTagId]->weight.count() < counting) {
            isWeighted = false;
        } else if (0 == store.tags[distanceShowTagId]->weight[counting].count()) {
            isWeighted = false;
        } else {
            weight = store.tags[distanceShowTagId]->weight[counting];
        }

        ui->label_Id->setText("<b><font black>----<\font><\b>");
        switch (cfgData.get_q()->sensor.count()) {
        case 6: IS_TEST_CHANGE_DISTANCE(5);
        case 5: IS_TEST_CHANGE_DISTANCE(4);
        case 4: IS_TEST_CHANGE_DISTANCE(3);
        case 3: IS_TEST_CHANGE_DISTANCE(2);
        case 2: IS_TEST_CHANGE_DISTANCE(1);
        case 1: IS_TEST_CHANGE_DISTANCE(0);
        default:
            RESET_SHOW_DIST_DATA(p);
        }

        if (isFaked) {
            oneTag &tmpTagData = fakeDistData.get_q()->tagsData[TEST_TAG_ID];
            ui->canvas->setDistance(TEST_TAG_ID, tmpTagData.distData[0].distance);
        }

        ui->canvas->syncWithUiFrame(usrFrame);
        isFaked ? fakeUsrFrame.show() : realUsrFrame.show();
        isFaked ? realUsrFrame.hide() : fakeUsrFrame.hide();

        resetUi(true, true);

        QAction *actionNowPos = getActionNowPos();
        if (actionNowPos) {
            actionNowPos->setChecked(true);
            if (ui->actionKalmanCoupled == actionNowPos) {
                kalmanCoupledSyncUi();
            }
        }

        int tagShowLM = usrFrame->getTagShowLM();
        oneUsrShowML(tagShowLM, tagShowLM != UN_INIT_LM_TAGID);

        setStatusTimeInfo();
        setStatusIter(-1, -1, -1, -1);
        handleModelDataUpdate(false);
    }
}
