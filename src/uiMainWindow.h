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

#include "showTagRelated.h"
#include "showStore.h"

#include "calcTagPos.h"
#include "calcTagNLOS.h"
#include "calcTagTrack.h"

#define SHOW_DIST_WEIGHT(n) \
    if ("1" == QString::number(oneTagInfo->weight[distCount][n])) {\
        ui->raw_##n->setStyleSheet("color:red");\
    } else {\
        ui->raw_##n->setStyleSheet("color:black");\
    }\
    ui->raw_##n->setText(QString("%1{%2}")\
            .arg(QString::number(tag.distData[distCount].distance[n]))\
            .arg(QString::number(oneTagInfo->weight[distCount][n]).left(4)));

#define SHOW_DIST_DIFF(n) \
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

    showStore store;

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
    void setStatusIter(int n, dType mse);

    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);

private slots:
    // FILE
    void loadIniConfigFile(bool checked, QString pathIn = "");
    void loadLogDistanceFile(bool checked, QString pathIn = "");
    void loadLogDistanceFile_2(bool checked, QString pathIn = "");
    void loadPictureFile(bool checked, QString pathIn = "");

    // NLOS
    void nlosWylie(bool checked);
    void nlosMultiPoint(bool checked);
    void nlosRes(bool checked);
    void nlosSumDist(bool checked);

    // POS
    void posCalcPROCESS(CALC_POS_TYPE type);
    void posFullCentroid(bool checked);
    void posSubLS(bool checked);
    void posTwoCenter(bool checked);
    void posTaylorSeries(bool checked);
    void posWeightedTaylor(bool checked);
    void posKalmanCoupled(bool checked);
    void posKalmanGauss(bool checked);
    void posKalmanWeight(bool checked);
    void posKalmanSmooth(bool checked);
    void posLMedS(bool checked);
    void posBilateration(bool checked);

    // TRACK
    void trackCalcPROCESS(TRACK_METHOD type);
    void trackKalman(bool checked);
    void trackKalmanLite(bool checked);
    void trackKalmanInfo(bool checked);

    // ZOOM
    void zoomIn(bool checked = false);
    void zoomOut(bool checked = false);
    void captureCanvas(bool checked = false);

    // ARM
    void posCalc_ARM(bool checked);

private slots:
    void handleModelDataUpdate(bool isUpdateCount = true);
    void oneUsrBtnClicked(int tagId, bool isShowable);
    void oneUsrShowML(int tagId, bool isShowML);

    void showMousePos(int x, int y);
};

#endif // MAINWINDOW_H
