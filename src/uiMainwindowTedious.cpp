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
            kalmanCoupledChange(true);
        } else if (actionNowPos == ui->actionKalmanCoupled && action != ui->actionKalmanCoupled) {
            kalmanCoupledChange(false);
        } else {}

        if (actionNowPos == action) {
            actionNowPos->setChecked(false);
            kalmanCoupledChange(false);
            actionNowPos = nullptr;
        } else {
            actionNowPos->setChecked(false);
            actionNowPos = action;
            actionNowPos->setChecked(true);
        }
    }
}

void uiMainWindow::kalmanCoupledChange(bool isEnable) {
    if (isEnable) {
        ui->actionKalmanTrail->setEnabled(true);
        ui->actionKalmanGauss->setEnabled(true);
        ui->actionKalmanWeight->setEnabled(true);
        ui->actionKalmanSmooth->setEnabled(true);
    } else {
        ui->actionKalmanTrail->setDisabled(true);
        ui->actionKalmanGauss->setDisabled(true);
        ui->actionKalmanWeight->setDisabled(true);
        ui->actionKalmanSmooth->setDisabled(true);
    }
}
void uiMainWindow::kalmanCoupledSyncUi() {
    ui->actionKalmanTrail->setChecked (calcPos.kalmanCoupledType & calcTagPos::TRAIL_COUPLED);
    ui->actionKalmanGauss->setChecked (calcPos.kalmanCoupledType & calcTagPos::GAUSS_COUPLED);
    ui->actionKalmanWeight->setChecked(calcPos.kalmanCoupledType & calcTagPos::WEIGHT_COUPLED);
    ui->actionKalmanSmooth->setChecked(calcPos.kalmanCoupledType & calcTagPos::SMOOTH_COUPLED);
    qDebug() << "[@uiMainWindow::kalmanCoupledSyncUi]" << calcPos.kalmanCoupledType << ";"
             << "TRAIL_COUPLED"  << (calcPos.kalmanCoupledType & calcTagPos::TRAIL_COUPLED) << ";"
             << "GAUSS_COUPLED"  << (calcPos.kalmanCoupledType & calcTagPos::GAUSS_COUPLED) << ";"
             << "WEIGHT_COUPLED" << (calcPos.kalmanCoupledType & calcTagPos::WEIGHT_COUPLED) << ";"
             << "SMOOTH_COUPLED" << (calcPos.kalmanCoupledType & calcTagPos::SMOOTH_COUPLED) << ";";
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
        distCount = 1;
        handleModelDataUpdate(false);
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        distCount --;
        handleModelDataUpdate(false);
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        distCount ++;
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

    connect(ui->distCountEdit, &QLineEdit::returnPressed, this, [this](void) {
        distCount = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
        ui->next->setFocus();
    });
    connect(ui->gotoCount, &QPushButton::clicked, this, [this](void) {
        distCount = ui->distCountEdit->text().toInt();
        handleModelDataUpdate(false);
    });

    connect(ui->UsrFrm, SIGNAL(oneUsrBtnClicked_siganl(int, bool)),
            this, SLOT(oneUsrBtnClicked(int, bool)));
    connect(ui->UsrFrm, SIGNAL(oneUsrShowML_siganl(int, bool)),
            this, SLOT(oneUsrShowML(int, bool)));

    // set Max Likehood's σ used in showTagRelated
    int sigmaInitValue = 250;
    ui->sigmaSlider->setValue(sigmaInitValue);
    sigmaChanged(sigmaInitValue);
    connect(ui->sigmaSlider, SIGNAL(valueChanged(int)), this, SLOT(sigmaChanged(int)));
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
        ui->actionTaylorSeries->setDisabled(true);

        ui->actionWeightedTaylor->setDisabled(true);
        ui->actionKalmanCoupled->setDisabled(true);
        ui->actionKalmanTrail->setDisabled(true);
        ui->actionKalmanGauss->setDisabled(true);
        ui->actionKalmanWeight->setDisabled(true);
        ui->actionKalmanSmooth->setDisabled(true);

        ui->actionLMedS->setDisabled(true);
        ui->actionBilateration->setDisabled(true);

        ui->actioncalcTagPos_ARM->setDisabled(true);

        ui->actionKalmanTrack->setDisabled(true);
        ui->actionKalmanLiteTrack->setDisabled(true);
        ui->actionKalmanInfoTrack->setDisabled(true);
    } else {
        ui->actionWylie->setEnabled(true);
        ui->actionMultiPoint->setEnabled(true);
        ui->actionRes->setEnabled(true);
        ui->actionSumDist->setEnabled(true);

        ui->actionFullCentroid->setEnabled(true);
        ui->actionSubLS->setEnabled(true);
        ui->actionTwoCenter->setEnabled(true);
        ui->actionTaylorSeries->setEnabled(true);

        ui->actionWeightedTaylor->setEnabled(true);
        ui->actionKalmanCoupled->setEnabled(true);
        ui->actionKalmanTrail->setEnabled(true);
        ui->actionKalmanGauss->setEnabled(true);
        ui->actionKalmanWeight->setEnabled(true);
        ui->actionKalmanSmooth->setEnabled(true);

        ui->actionLMedS->setEnabled(true);
        ui->actionBilateration->setEnabled(true);

        ui->actioncalcTagPos_ARM->setEnabled(true);

        ui->actionKalmanTrack->setEnabled(true);
        ui->actionKalmanLiteTrack->setEnabled(true);
        ui->actionKalmanInfoTrack->setEnabled(true);
    }
}
void uiMainWindow::resetData() {
    // CLEAR LEGACY DATA
    store.clear();
    distData.clear();
    ui->UsrFrm->removeAll();
    ui->canvas->removeAll();
    // CLEAR UI
    resetUi(true, true);
}

void uiMainWindow::resetUi(bool isPos, bool isTrack) {
    if (isPos) {
        calcPos.calcPosType = CALC_POS_TYPE::POS_NONE;
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
        calcTrack.calcTrackMethod = TRACK_METHOD::TRACK_NONE;
        ui->actionKalmanTrack->setChecked(false);
        ui->actionKalmanLiteTrack->setChecked(false);
        ui->actionKalmanInfoTrack->setChecked(false);
    } else {}
}

void uiMainWindow::setStatusTimeInfo() {
    QString posStr;
    QString trackStr;
    if (CALC_POS_TYPE::POS_NONE == calcPos.calcPosType) {
        posStr = "{pos_none}";
    } else {
        posStr = CALC_POS2STR[calcPos.calcPosType];
    }
    if (TRACK_METHOD::TRACK_NONE == calcTrack.calcTrackMethod) {
        trackStr = "{track_none}";
    } else {
        trackStr = TRACK_METHOD2STR[calcTrack.calcTrackMethod];
    }
    calcTimeElapsed->setText(QString("<b>nPOS:%0</b> | <b>%1</b>:%2(s) | <b>%3</b>:%4(s)")
                             .arg(totalPos)
                             .arg(posStr).arg(calcTimeElapsedMeasu)
                             .arg(trackStr).arg(calcTimeElapsedTrack));
}
void uiMainWindow::setStatusDistCount() {
    distCountShow->setText(QString("distCount: <b>%0</b>").arg(distCount, 4, 10, QChar('0')));
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
}

void uiMainWindow::setStatusIter(int n, dType mse) {
    iterationNum->setText(QString::number(n) + QString("<") + QString::number((mse)) + QString(">"));
}
