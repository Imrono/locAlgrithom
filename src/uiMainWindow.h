﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "_myheads.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QAction>
#include <QLabel>
#include "dataType.h"
#include "uiUsrFrame.h"

#include "dataInput_calcAnalyzeSet.h"
#include "dataInputPos.h"
#include "dataInputCan.h"
#include "dataInputLog.h"
#include "dataInputTest.h"
#include "dataSensorIni.h"

#include "showTagDelegate.h"
#include "showTagModel.h"

#include "calcTagPos.h"
#include "calcTagNLOS.h"
#include "calcTagTrack.h"

#define IS_TEST_CHANGE_DISTANCE(n, ui, isEnable) \
    ui->raw_##n->setStyleSheet("color:black");\
    ui->weight_##n->setStyleSheet("color:black");\
    ui->raw_##n->setEnabled(isEnable);\
    ui->raw_##n->setText(QString::number(distance[n]));\
    ui->weight_##n->setText("NaN");\
    ui->refine_##n->setText("NaN");\


#define SET_DISTANCE(n) \
    if (calcAnalyzeInput[CALC_TEST_ANALYZE]->isActive) {\
        int dist = ui->raw_n->text().toInt();\
        dataInputTest *dataInput = static_cast<dataInputTest *>(calcAnalyzeInput[CALC_TEST_ANALYZE]->dataInputHandler);\
        if (dataInput->oneDistData.distance.count() < 6) {\
            dataInput->oneDistData.distance = QVector<int>(6, 0);\
        }\
        dataInput->oneDistData.distance[n] = dist;\
        ui->canvas->setDistance(TEST_TAG_ID, dataInput->oneDistData.distance);\
    }

#define RESET_SHOW_DIST_DATA(n, ui) \
    ui->raw_##n->setText(QString("0"));\
    ui->weight_##n->setText(QString("0"));\
    ui->refine_##n->setText(QString("0"));\

#define SHOW_DIST_DATA(n, weight, distance) \
    if ("1" == QString::number(weight[n])) {\
        ui->raw_##n->setStyleSheet("color:red");\
        ui->weight_##n->setStyleSheet("color:red");\
    } else {\
        ui->raw_##n->setStyleSheet("color:black");\
        ui->weight_##n->setStyleSheet("color:black");\
    }\
    ui->raw_##n->setText   (QString::number(distance[n]));\
    ui->weight_##n->setText(QString::number(weight[n], 'f', 3).left(5));\
    ui->refine_##n->setText(QString::number(\
    qAbs(calcDistance(tag->methodInfo[MEASUR_STR].Ans[counting], cfgData.get_q()->sensor[n]) - distance[n])))


namespace Ui {
class MainWindow;
}

class uiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit uiMainWindow(QWidget *parent = 0);
    ~uiMainWindow();

    friend class dataInputPos;
    friend class dataInputCan;
    friend class dataInput_calcAnalyzeSet;

    enum ANALYZE_STATUS {
        CALC_LOG_ANALYZE,
        CALC_TEST_ANALYZE,
        CALC_CAN_ANALYZE,
        CALC_POS_ANALYZE,
        NUM_ANALYZE_STATUS
    };

private:
    Ui::MainWindow *ui;

    ANALYZE_STATUS analyzeStatus;
    dataInput_calcAnalyzeSet *calcAnalyzeInput[NUM_ANALYZE_STATUS];

    QTimer stepShowTimer;
    bool stepShowTimerStarted;

    int &getCounting() {
        return calcAnalyzeInput[analyzeStatus]->counting;
    }

    calcTagPos calcPos;
    calcTagNLOS *calcNlos;
    calcTagTrack calcTrack;

    // sensor location and oper, alarm, stop
    dataSensorIni cfgData;

    showTagModel *workingStore{nullptr};
    uiUsrFrame *workingUsrFrame{nullptr};

    QAction *actionNowPos{nullptr};
    QAction *actionNowTrack{nullptr};

    showTagModel &getStore() {
        return calcAnalyzeInput[analyzeStatus]->modelStore;
    }
    uiUsrFrame &getUsrFrame() {
        return calcAnalyzeInput[analyzeStatus]->usrFrame;
    }

    void checkData();
    void resetData();
    void resetUi(bool isPos = false, bool isTrack = false);
    void connectUi();

    QLabel *calcTimeElapsed{nullptr};
    void setStatusTimeInfo(bool isCalcPosAnalyze = false);
    QLabel *distCountShow{nullptr};
    void setStatusDistCount();
    QLabel *distZoomShow{nullptr};
    void setStatusZoom();
    QLabel *canvasPosShow{nullptr};
    void setStatusMousePos(int x, int y);
    QLabel *iterationNum{nullptr};
    void setStatusIter(int n, dType mse, int crossed1 = 0, int crossed2 = 0);

    void wheelEvent(QWheelEvent *e);

    void kalmanCoupledChange(bool isEnable);
    void kalmanCoupledSyncUi();

    int distanceShowTagId{UN_INIT_SHOW_TAGID};
    QString lastIniPath;
    QString lastDistancePath;

    void syncTestDistanceData();

private slots:
    // FILE
    void loadIniConfigFile(QString pathIn = "");
    void loadLogDistanceFile(int type, QString pathIn = "");
    void loadPictureFile(QString pathIn = "");

    // NLOS
    void nlosWylie();
    void nlosMultiPoint();
    void nlosRes();
    void nlosSumDist();

    // POS
    void posCalcPROCESS(CALC_POS_TYPE type, int tagId = 0);
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
    void reflashUI();

    void handleModelDataUpdate(bool isUpdateCount = true, bool isShowRT = false);
    void oneUsrBtnClicked(int tagId, bool isShowable);
    void oneUsrShowML(int tagId, bool isShowML);
    void sigmaChanged(int sigma);

    void showMousePos(int x, int y);

    void on_cbAnalyzeMode_currentIndexChanged(int index);
    void on_refresh_clicked();
};

#endif // MAINWINDOW_H
