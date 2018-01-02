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
    qDebug() << QString("distCount:%0, POSITION(%1,%2,%3)->KALMAN(%4,%5,%6) => d=%7, totDist=%8, R=%9, P=%10"
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

    //ui->raw_0->setText(QString::number(meas->));

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
    calcPos.setConfigData(cfgData.get_q());
    ui->canvas->setConfigData(cfgData.get_q());
    ui->actionRead_ini->setChecked(true);
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
    calcNlos.predictNlos = calcTagNLOS::WYLIE;
    ui->actionWylie->setChecked(true);
    ui->actionMultiPoint->setChecked(false);
    qDebug() << "nlosWylie";
}

void uiMainWindow::nlosMultiPoint(bool checked) {
    Q_UNUSED(checked);
    calcNlos.predictNlos = calcTagNLOS::MULTI_POINT;
    ui->actionWylie->setChecked(false);
    ui->actionMultiPoint->setChecked(true);
    qDebug() << "nlosMultiPoint";
}

void uiMainWindow::nlosRes(bool checked) {
    Q_UNUSED(checked);
    calcNlos.precNlos = calcTagNLOS::RESIDUAL;
    ui->actionRes->setChecked(true);
    ui->actionSumDist->setChecked(false);
    qDebug() << "nlosRes";
}

void uiMainWindow::nlosSumDist(bool checked) {
    Q_UNUSED(checked);
    calcNlos.precNlos = calcTagNLOS::SUM_DIST;
    ui->actionRes->setChecked(false);
    ui->actionSumDist->setChecked(true);
    qDebug() << "nlosSumDist";
}

// POSITION
void uiMainWindow::posFullCentroid(bool checked) {
    Q_UNUSED(checked);
    ui->actionFullCentroid->setChecked(true);
    ui->actionSubLS->setChecked(false);
    ui->actionTwoCenter->setChecked(false);
    calcPos.calcPosType = calcTagPos::FullCentroid;

    store.getLabel(MEASUR_STR)->resetTrack();
    calcPos.calcPosVectorKang(store.getLabel(MEASUR_STR));
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
    calcPos.calcPosType = calcTagPos::SubLS;

    store.getLabel(MEASUR_STR)->resetTrack();
    //calc.calcPosVector_2(store.getLabel(MEASUR_STR));
    calcPos.calcPosVectorKang(store.getLabel(MEASUR_STR));

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
    calcPos.calcPosType = calcTagPos::TwoCenter;

    store.getLabel(MEASUR_STR)->resetTrack();
    //calc.calcPosVector_2(store.getLabel(MEASUR_STR));
    calcPos.calcPosVectorKang(store.getLabel(MEASUR_STR));

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
