#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "_myheads.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QAction>
#include <QLabel>
#include "dataType.h"
#include "uiUsrFrame.h"

#include "dataMpPos.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"

#include "showTagDelegate.h"
#include "showTagModel.h"

#include "calcTagPos.h"
#include "calcTagNLOS.h"
#include "calcTagTrack.h"

#define IS_TEST_CHANGE_DISTANCE(n) \
    ui->raw_##n->setStyleSheet("color:black");\
    ui->raw_##n->setEnabled(isFaked);\
    ui->raw_##n->setText(QString::number(distance[n]));\
    if (!isWeighted) {\
        ui->weight_##n->setText("NaN");\
        ui->refine_##n->setText("NaN");\
    }

#define SET_DISTANCE(n) \
    if (isFaked) {\
        oneTag &tmpTagData = fakeDistData.get_q()->tagsData[TEST_TAG_ID];\
        int dist = ui->raw_##n->text().toInt();\
        tmpTagData.distData[0].distance[n] = dist;\
        tmpTagData.distData[1].distance[n] = dist;\
        ui->canvas->setDistance(TEST_TAG_ID, tmpTagData.distData[0].distance);\
    }

#define RESET_SHOW_DIST_DATA(n) \
    ui->raw_##n->setText(QString("0"));\
    ui->weight_##n->setText(QString("0"));\
    ui->refine_##n->setText(QString("0"));\

#define SHOW_DIST_DATA(n) \
    if ("1" == QString::number(oneTagInfo->weight[counting][n])) {\
        ui->raw_##n->setStyleSheet("color:red");\
        ui->weight_##n->setStyleSheet("color:red");\
    } else {\
        ui->raw_##n->setStyleSheet("color:black");\
        ui->weight_##n->setStyleSheet("color:black");\
    }\
    ui->raw_##n->setText   (QString::number(tag.distData[counting].distance[n]));\
    ui->weight_##n->setText(QString::number(oneTagInfo->weight[counting][n], 'f', 3).left(5));\
    ui->refine_##n->setText(QString::number(qAbs(\
    calcDistance(oneTagInfo->methodInfo[MEASUR_STR].Ans[counting], cfgData.get_q()->sensor[n])\
    - tag.distData[counting].distance[n])))
// ui->refine_##n->setText(QString::number(\
// calcDistance(tag.distData[counting].p_t, cfgData.get_q()->sensor[n])))

namespace Ui {
class MainWindow;
}

class uiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit uiMainWindow(QWidget *parent = 0);
    ~uiMainWindow();

    friend class dataMpPos;

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool timerStarted;

    int realCounting{0};
    int fakeCounting{1};
    int &getCounting() {
        return !isFaked ? realCounting : fakeCounting;
    }

    calcTagPos calcPos;
    calcTagNLOS calcNlos;
    calcTagTrack calcTrack;

    dataMpPos mpPosInput;
    bool isMpPosInput{false};

    // sensor location and oper, alarm, stop
    dataSensorIni cfgData;

    bool isFaked{false};
    dataDistanceLog realDistData;
    showTagModel realStore;     //store result

    dataDistanceLog fakeDistData;
    showTagModel fakeStore;     //store result

    uiUsrFrame fakeUsrFrame;
    uiUsrFrame realUsrFrame;

    QAction *actionRealNowPos{nullptr};
    QAction *actionFakeNowPos{nullptr};

    dataDistanceLog &getDistData() {
        return !isFaked ? realDistData : fakeDistData;
    }
    showTagModel &getStore() {
        return !isFaked ? realStore : fakeStore;
    }
    uiUsrFrame &getUsrFrame() {
        return !isFaked ? realUsrFrame : fakeUsrFrame;
    }
    QAction *&getActionNowPos() {
        return !isFaked ? actionRealNowPos : actionFakeNowPos;
    }

    void initWithDistanceData();

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

    QAction *actionNowTrack{nullptr};

    void kalmanCoupledChange(bool isEnable);
    void kalmanCoupledSyncUi();

    void reflashUI();

    int distanceShowTagId{UN_INIT_SHOW_TAGID};
    QString lastIniPath;
    QString lastDistancePath;

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

    void modelChange(bool isTest);
    void on_btn_mpPos_clicked();
};

#endif // MAINWINDOW_H
