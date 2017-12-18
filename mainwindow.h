#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include "datatype.h"
#include "calcPos.h"
#include "showPoint.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(calcPos calcIn, QWidget *parent = 0);
    ~MainWindow();

    QVector<labelDistance> dist;
    QVector<QVector<locationCoor>>  pos;
    QVector<locationCoor>  kalmanPos;
    QVector<QLine>         lines;
    QVector<QLine>         kalmanLines;

    QVector<locationCoor> calcFromDist(uint32_t dist[], uint32_t count = 4);
    void calcPosVector();
    double calcTotalDistanceMeas();
    locationCoor calcKalmanFromPos(uint32_t dist[], uint32_t count = 4);
    void calcKalmanPosVector(double Q_in = 0.014f);
    double calcTotalDistanceKalman();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    QVector<showPoint> labels;
    calcPos calc;
    int distCount{1};

    void paintEvent(QPaintEvent *event);

private slots:
    void handleTimeout();
};

#endif // MAINWINDOW_H
