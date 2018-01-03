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

    store.appendLabel(MEASUR_STR,
                      showTagRelated(4, QPen(QColor(0, 160, 230), 2), QBrush(QColor(255, 160, 90)),
                                   QPen(Qt::black, 1), QBrush(QColor(255, 160, 90))));
    store.appendLabel(KALMAN_STR, showTagRelated(4, QPen(Qt::gray, 2), QBrush(Qt::darkGreen)));

    // DATA
    cfgData.loadNewFile("D:\\code\\kelmanLocationData\\configExample.ini");
    qDebug() << cfgData.toString();
    calcPos.setConfigData(cfgData.get_q());
    distData.loadNewFile("D:\\code\\kelmanLocationData\\201712111515.log");
    qDebug() << distData.toString();
    calcPos.setDistanceData(distData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());
    calcPos.setNlosJudge(&calcNlos);

    // FILE
    connect(ui->actionRead_ini,  SIGNAL(triggered(bool)), this, SLOT(loadIniConfigFile(bool)));
    connect(ui->actionRead_dist, SIGNAL(triggered(bool)), this, SLOT(loadLogDistanceFile(bool)));

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
            ui->canvas->setLines(MEASUR_STR, store.getLabel(MEASUR_STR)->AnsLines);
            ui->canvas->setLines(KALMAN_STR, store.getLabel(KALMAN_STR)->AnsLines);
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
            ui->allPos->setText(MY_STR("显示半径"));
        } else {
            ui->allPos->setText(MY_STR("隐藏半径"));
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
    //nlosRes(true);
    //nlosMultiPoint(true);
    //posSubLS(true);
    //trackKalman(true);
}

uiMainWindow::~uiMainWindow()
{
    delete ui;
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

    labelInfo *meas = store.getLabel(MEASUR_STR);
    labelInfo *kalm = store.getLabel(KALMAN_STR);

    locationCoor p_meas = meas->Ans[distCount];
    locationCoor p_kalm = kalm->Ans[distCount];
    qDebug() << QString("distCount:%0, POS(%1,%2,%3)->KALMAN(%4,%5,%6)=>d=%7, totDist=%8, R=%9, P=%10"
                        ", K=%12")
                .arg(distCount, 4, 10, QChar('0'))
                .arg(p_meas.x,4,'g',3).arg(p_meas.y,4,'g',3).arg(p_meas.z,4,'g',3)
                .arg(p_kalm.x,4,'g',3).arg(p_kalm.y,4,'g',3).arg(p_kalm.z,4,'g',3)
                .arg(calcDistance(p_meas, p_kalm),4,'g',3)
                .arg(meas->Reliability[distCount],5,'g',3)
                .arg(kalm->data_R[distCount],5,'g',3)
                .arg(kalm->data_P[distCount],5,'g',3)
                .arg(kalm->Reliability[distCount],5,'g',3);

    ui->canvas->setPosition(MEASUR_STR, meas->Ans[distCount].toQPointF());
    ui->canvas->setLine(MEASUR_STR, meas->AnsLines[distCount-1]);
    ui->canvas->setPosition(KALMAN_STR, kalm->Ans[distCount].toQPointF());
    ui->canvas->setLine(KALMAN_STR, kalm->AnsLines[distCount-1]);

    ui->canvas->setPointsRaw(MEASUR_STR, meas->RawPoints[distCount]);
    ui->canvas->setPointsRefined(MEASUR_STR, meas->RefinedPoints[distCount]);

    ui->canvas->setDistance(MEASUR_STR, distData.get_q()->dist[distCount].distance);

    ui->raw_0->setText(QString::number(distData.get_q()->dist[distCount].distance[0]));
    ui->raw_1->setText(QString::number(distData.get_q()->dist[distCount].distance[1]));
    ui->raw_2->setText(QString::number(distData.get_q()->dist[distCount].distance[2]));
    ui->raw_3->setText(QString::number(distData.get_q()->dist[distCount].distance[3]));

    if (distData.get_q()->dist[distCount].distance[0] != calcPos.distRefined[distCount].distance[0]) {
        ui->refine_0->setStyleSheet("color:red; font-weight:bold");
    } else {
        ui->refine_0->setStyleSheet("");
    }
    if (distData.get_q()->dist[distCount].distance[1] != calcPos.distRefined[distCount].distance[1]) {
        ui->refine_1->setStyleSheet("color:red; font-weight:bold");
    } else {
        ui->refine_1->setStyleSheet("");
    }
    if (distData.get_q()->dist[distCount].distance[2] != calcPos.distRefined[distCount].distance[2]) {
        ui->refine_2->setStyleSheet("color:red; font-weight:bold");
    } else {
        ui->refine_2->setStyleSheet("");
    }
    if (distData.get_q()->dist[distCount].distance[3] != calcPos.distRefined[distCount].distance[3]) {
        ui->refine_3->setStyleSheet("color:red; font-weight:bold");
    } else {
        ui->refine_3->setStyleSheet("");
    }
    ui->refine_0->setText(QString::number(calcPos.distRefined[distCount].distance[0]));
    ui->refine_1->setText(QString::number(calcPos.distRefined[distCount].distance[1]));
    ui->refine_2->setText(QString::number(calcPos.distRefined[distCount].distance[2]));
    ui->refine_3->setText(QString::number(calcPos.distRefined[distCount].distance[3]));

    update();

    if (distCount == meas->Ans.count())
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
    qDebug() << "ini File loaded1111";
    calcPos.setConfigData(cfgData.get_q());
    qDebug() << "ini File loaded2222";
    ui->canvas->setConfigData(cfgData.get_q());
    ui->actionRead_ini->setChecked(true);
    qDebug() << "ini File loaded";
}
void uiMainWindow::loadLogDistanceFile(bool checked) {
    Q_UNUSED(checked);
    QString path = QFileDialog::getOpenFileName(this, "Select Distance Log File", ".", "distance file(*.log)");
    qDebug() << "loadLogDistanceFile Path:" << path;
    distData.loadNewFile(path);
    calcPos.setDistanceData(distData.get_q());
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

    store.getLabel(MEASUR_STR)->resetTrack();
    calcPos.calcPosVector(store.getLabel(MEASUR_STR));
    qDebug() << "posFullCentroid:" << store.getLabel(MEASUR_STR)->toString();
    dType measDist = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    qDebug() << "posFullCentroid: avgDistanceSquare => measDist:" << measDist;

    ui->canvas->setLines(MEASUR_STR, store.getLabel(MEASUR_STR)->AnsLines);
    update();
}

void uiMainWindow::posSubLS(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(false);
    ui->actionSubLS->setChecked(true);
    ui->actionTwoCenter->setChecked(false);
    calcPos.calcPosType = CALC_POS_TYPE::SubLS;

    store.getLabel(MEASUR_STR)->resetTrack();
    //calc.calcPosVector_2(store.getLabel(MEASUR_STR));
    calcPos.calcPosVector(store.getLabel(MEASUR_STR));

    qDebug() << "posSubLS:" << store.getLabel(MEASUR_STR)->toString();
    dType measDist = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    qDebug() << "posSubLS: avgDistanceSquare => measDist:" << measDist;

    ui->canvas->setLines(MEASUR_STR, store.getLabel(MEASUR_STR)->AnsLines);
    update();
}

void uiMainWindow::posTwoCenter(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(false);
    ui->actionSubLS->setChecked(false);
    ui->actionTwoCenter->setChecked(true);
    calcPos.calcPosType = CALC_POS_TYPE::TwoCenter;

    store.getLabel(MEASUR_STR)->resetTrack();
    //calc.calcPosVector_2(store.getLabel(MEASUR_STR));
    calcPos.calcPosVector(store.getLabel(MEASUR_STR));

    qDebug() << "posTwoCenter:" << store.getLabel(MEASUR_STR)->toString();
    dType measDist = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    qDebug() << "posTwoCenter: avgDistanceSquare => measDist:" << measDist;

    ui->canvas->setLines(MEASUR_STR, store.getLabel(MEASUR_STR)->AnsLines);
    update();
}

// TRACK
void uiMainWindow::trackKalman(bool checked) {
    Q_UNUSED(checked);
    ui->actionKalmanTrack->setChecked(true);
    ui->actionkalmanLiteTrack->setChecked(false);

    store.getLabel(KALMAN_STR)->resetTrack();
    calcKalman::calcKalmanPosVector(store.getLabel(MEASUR_STR), store.getLabel(KALMAN_STR));

    qDebug() << store.getLabel(KALMAN_STR)->toString();
    dType measDist   = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    dType kalmanDist = calcTotalAvgDistanceSquare(store.getLabel(KALMAN_STR)->AnsLines);
    qDebug() << "kalmanTrack: avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

    ui->canvas->setLines(KALMAN_STR, store.getLabel(KALMAN_STR)->AnsLines);
    update();
}
void uiMainWindow::trackKalmanLite(bool checked) {
    Q_UNUSED(checked);
    ui->actionKalmanTrack->setChecked(false);
    ui->actionkalmanLiteTrack->setChecked(true);

    store.getLabel(KALMAN_STR)->resetTrack();
    calcKalman::calcKalmanPosVectorLite(store.getLabel(MEASUR_STR), store.getLabel(KALMAN_STR));

    qDebug() << store.getLabel(KALMAN_STR)->toString();
    dType measDist   = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    dType kalmanDist = calcTotalAvgDistanceSquare(store.getLabel(KALMAN_STR)->AnsLines);
    qDebug() << "kalmanTrackLite: avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

    ui->canvas->setLines(KALMAN_STR, store.getLabel(KALMAN_STR)->AnsLines);
    update();
}
