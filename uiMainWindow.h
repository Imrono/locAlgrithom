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

    dataSensorIni cfgData;
    dataDistanceLog distData;

    void paintEvent(QPaintEvent *event);
    void checkData();

private slots:
    // FILE
    void loadIniConfigFile(bool checked);
    void loadLogDistanceFile(bool checked);
    void loadLogDistanceFile_2(bool checked);
    void loadPictureFile(bool checked);

    // NLOS
    void nlosWylie(bool checked);
    void nlosMultiPoint(bool checked);
    void nlosRes(bool checked);
    void nlosSumDist(bool checked);

    // POS
    void posFullCentroid(bool checked);
    void posSubLS(bool checked);
    void posTwoCenter(bool checked);

    // TRACK
    void trackKalman(bool checked);
    void trackKalmanLite(bool checked);

private slots:
    void handleTimeout(bool isUpdateCount = true);
    void oneUsrBtnClicked(int tagId, bool isShowable);
};

#endif // MAINWINDOW_H
