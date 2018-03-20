#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "_myheads.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QAction>
#include <QLabel>
#include "dataType.h"

#include "dataSensorIni.h"
#include "dataDistanceLog.h"

#include "showTagDelegate.h"
#include "showTagModel.h"

#include "calcTagPos.h"
#include "calcTagNLOS.h"
#include "calcTagTrack.h"

#define SHOW_DIST_DATA(n) \
    if ("1" == QString::number(oneTagInfo->weight[distCount][n])) {\
        ui->raw_##n->setStyleSheet("color:red");\
    } else {\
        ui->raw_##n->setStyleSheet("color:black");\
    }\
    ui->raw_##n->setText(QString("%1{%2}")\
            .arg(QString::number(tag.distData[distCount].distance[n]))\
            .arg(QString::number(oneTagInfo->weight[distCount][n]).left(4)));\
    ui->refine_##n->setText(QString::number(qAbs(\
    calcDistance(oneTagInfo->methodInfo[MEASUR_STR].Ans[distCount], cfgData.get_q()->sensor[n])\
    - tag.distData[distCount].distance[n])))
// ui->refine_##n->setText(QString::number(\
// calcDistance(tag.distData[distCount].p_t, cfgData.get_q()->sensor[n])))

namespace Ui {
class MainWindow;
}

class uiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit uiMainWindow(QWidget *parent = 0);
    ~uiMainWindow();

    showTagModel store;

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool timerStarted;

    int distCount{0};

    calcTagPos calcPos;
    calcTagNLOS calcNlos;
    calcTagTrack calcTrack;

    dataSensorIni cfgData;
    dataDistanceLog distData;

    int totalPos{0};
    dType calcTimeElapsedMeasu{0.f};
    dType calcTimeElapsedTrack{0.f};

    void checkData();
    void resetData();
    void resetUi(bool isPos = false, bool isTrack = false);
    void connectUi();

    QLabel *calcTimeElapsed{nullptr};
    void setStatusTimeInfo();
    QLabel *distCountShow{nullptr};
    void setStatusDistCount();
    QLabel *distZoomShow{nullptr};
    void setStatusZoom();
    QLabel *canvasPosShow{nullptr};
    void setStatusMousePos(int x, int y);
    QLabel *iterationNum{nullptr};
    void setStatusIter(int n, dType mse, int crossed1 = 0, int crossed2 = 0);

    void wheelEvent(QWheelEvent *e);

    QAction *actionNowPos{nullptr};
    QAction *actionNowTrack{nullptr};

    void kalmanCoupledChange(bool isEnable);
    void kalmanCoupledSyncUi();

    int distanceShowTagId{-1};
private slots:
    // FILE
    void loadIniConfigFile(QString pathIn = "");
    void loadLogDistanceFile(QString pathIn = "");
    void loadLogDistanceFile_2(QString pathIn = "");
    void loadPictureFile(QString pathIn = "");

    // NLOS
    void nlosWylie();
    void nlosMultiPoint();
    void nlosRes();
    void nlosSumDist();

    // POS
    void posCalcPROCESS(CALC_POS_TYPE type);
    void posFullCentroid();
    void posSubLS();
    void posTwoCenter();
    void posTaylorSeries();
    void posWeightedTaylor();
    void posKalmanCoupled();
    void posKalmanTrail();
    void posKalmanGauss();
    void posKalmanWeight();
    void posKalmanSmooth();
    void posLMedS();
    void posBilateration();
    void UPDATE_POS_UI(QAction *action);

    // TRACK
    void trackCalcPROCESS(TRACK_METHOD type);
    void trackKalman();
    void trackKalmanLite();
    void trackKalmanInfo();
    void UPDATE_TRACK_UI(QAction *action);

    // ZOOM
    void zoomIn(bool checked = false);
    void zoomOut(bool checked = false);
    void captureCanvas(bool checked = false);

    // ARM
    void posCalc_ARM();

private slots:
    void handleModelDataUpdate(bool isUpdateCount = true);
    void oneUsrBtnClicked(int tagId, bool isShowable);
    void oneUsrShowML(int tagId, bool isShowML);
    void sigmaChanged(int sigma);

    void showMousePos(int x, int y);
};

#endif // MAINWINDOW_H
