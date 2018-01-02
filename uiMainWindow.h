#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "_myheads.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QAction>
#include "datatype.h"

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

signals:
    void countChanged(int cnt);

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool timerStarted;

    int distCount{0};

    calcTagPos calcPos;
    calcTagNLOS calcNlos;

    dataSensorIni cfgData;
    dataDistanceLog distData;

    void paintEvent(QPaintEvent *event);

private slots:
    // FILE
    void loadIniConfigFile(bool checked);
    void loadLogDistanceFile(bool checked);

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
    void handleTimeout();
};

#endif // MAINWINDOW_H
