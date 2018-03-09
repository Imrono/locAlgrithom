#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QClipboard>

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
    connect(ui->actionFullCentroid,    SIGNAL(triggered(bool)), this, SLOT(posFullCentroid(bool)));
    connect(ui->actionSubLS,           SIGNAL(triggered(bool)), this, SLOT(posSubLS(bool)));
    connect(ui->actionTwoCenter,       SIGNAL(triggered(bool)), this, SLOT(posTwoCenter(bool)));
    connect(ui->actionTaylorSeries,    SIGNAL(triggered(bool)), this, SLOT(posTaylorSeries(bool)));

    connect(ui->actionWeightedTaylor,  SIGNAL(triggered(bool)), this, SLOT(posWeightedTaylor(bool)));
    connect(ui->actionKalmanLoose,     SIGNAL(triggered(bool)), this, SLOT(posKalmanLoose(bool)));
    connect(ui->actionKalmanMedium,    SIGNAL(triggered(bool)), this, SLOT(posKalmanMedium(bool)));
    connect(ui->actionKalmanTight,     SIGNAL(triggered(bool)), this, SLOT(posKalmanTight(bool)));
    connect(ui->actionKalmanUltraTight,SIGNAL(triggered(bool)), this, SLOT(posKalmanUltraTight(bool)));

    connect(ui->actionLMedS,           SIGNAL(triggered(bool)), this, SLOT(posLMedS(bool)));
    connect(ui->actionBilateration,    SIGNAL(triggered(bool)), this, SLOT(posBilateration(bool)));

    // TRACK
    connect(ui->actionKalmanTrack,     SIGNAL(triggered(bool)), this, SLOT(trackKalman(bool)));
    connect(ui->actionKalmanLiteTrack, SIGNAL(triggered(bool)), this, SLOT(trackKalmanLite(bool)));
    connect(ui->actionKalmanInfoTrack, SIGNAL(triggered(bool)), this, SLOT(trackKalmanInfo(bool)));

    // ZOOM
    connect(ui->actionZoomIn,  SIGNAL(triggered(bool)), this, SLOT(zoomIn(bool)));
    connect(ui->actionZoomOut, SIGNAL(triggered(bool)), this, SLOT(zoomOut(bool)));

    // CANVAS CAPTURE PICTURE
    connect(ui->actionCapPix, SIGNAL(triggered(bool)), this, SLOT(captureCanvas(bool)));

    // ARM
    connect(ui->actioncalcTagPos_ARM, SIGNAL(triggered(bool)), this, SLOT(posCalc_ARM(bool)));
/*************************************************************/
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
}

void uiMainWindow::keyPressEvent(QKeyEvent *e) {
//    if (e->modifiers() == Qt::ShiftModifier) {
//        if (e->key() == Qt::Key_Less) {
//            emit ui->previous->clicked();
//        } else if (e->key() == Qt::Key_Greater) {
//            emit ui->next->clicked();
//        } else {}
//    } else {}
    QMainWindow::keyPressEvent(e);
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
        ui->actionKalmanLoose->setDisabled(true);
        ui->actionKalmanMedium->setDisabled(true);
        ui->actionKalmanTight->setDisabled(true);
        ui->actionKalmanUltraTight->setDisabled(true);

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
        ui->actionKalmanLoose->setEnabled(true);
        ui->actionKalmanMedium->setEnabled(true);
        ui->actionKalmanTight->setEnabled(true);
        ui->actionKalmanUltraTight->setEnabled(true);

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
        ui->actionKalmanLoose->setChecked(false);
        ui->actionKalmanMedium->setChecked(false);
        ui->actionKalmanTight->setChecked(false);
        ui->actionKalmanUltraTight->setChecked(false);

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
    iterationNum->setText(QString::number((n)) + QString("<") + QString::number((mse)) + QString(">"));
}
