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
    QLabel *distCountShow{nullptr};

    calcTagPos calcPos;
    calcTagNLOS calcNlos;
    calcTagTrack calcTrack;

    dataSensorIni cfgData;
    dataDistanceLog distData;

    int totalPos{0};
    dType calcTimeElapsedMeasu{0.f};
    dType calcTimeElapsedTrack{0.f};
    QLabel *calcTimeElapsed{nullptr};

    QLabel *distZoomShow{nullptr};
    QLabel *canvasPosShow{nullptr};

    void checkData();
    void resetData();
    void resetUi(bool isPos = false, bool isTrack = false);
    void connectUi();

    void setStatusTimeInfo();
    void setStatusDistCount();
    void setStatusZoom();
    void setStatusMousePos(int x, int y);

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
    void posKalmanTaylor(bool checked);

    // TRACK
    void trackCalcPROCESS(TRACK_METHOD type);
    void trackKalman(bool checked);
    void trackKalmanLite(bool checked);
    void trackKalmanInfo(bool checked);

    // ZOOM
    void zoomIn(bool checked = false);
    void zoomOut(bool checked = false);

private slots:
    void handleModelDataUpdate(bool isUpdateCount = true);
    void oneUsrBtnClicked(int tagId, bool isShowable);

    void showMousePos(int x, int y);
};

#endif // MAINWINDOW_H
