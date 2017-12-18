#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include "kalmanCalc.h"

MainWindow::MainWindow(calcPos calcIn, QWidget *parent) :
    calc(calcIn), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    labels.append({0,0});
    labels.append({0,0});
    showLine.append({{0,0}, {0,0}});
    showLine.append({{0,0}, {0,0}});
    qDebug() << "labelCount:" << labels.count() << "showLine:" << showLine.count();

    connect(&timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    //timer.start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 设置画笔颜色、宽度
    painter.setPen(QPen(Qt::gray, 0));
    // 设置画刷颜色
    painter.setBrush(Qt::gray);
    painter.drawRect(QRect(185, 140, 30, 20));

    for (int i = 0; i < 4; i++) {
        painter.setPen(QPen(QColor(0, 0, 0), 2));
        painter.setBrush(QColor(0, 0, 0));
        QPoint p(calc.getLoc(i).x, calc.getLoc(i).y);
        painter.drawEllipse(p, 3, 3);
        painter.setPen(QColor(0, 160, 230));
        if (0 == i) {
            painter.drawText(p.rx()-10, p.ry()-10, 10, 10, Qt::AlignCenter, QString::number(i, 10));
        } else if (1 == i) {
            painter.drawText(p.rx(), p.ry()-10, 10, 10, Qt::AlignCenter, QString::number(i, 10));
        } else if (2 == i) {
            painter.drawText(p.rx(), p.ry(), 10, 10, Qt::AlignCenter, QString::number(i, 10));
        } else if (3 == i) {
            painter.drawText(p.rx()-10, p.ry(), 10, 10, Qt::AlignCenter, QString::number(i, 10));
        } else {}
    }

    painter.setRenderHint(QPainter::Antialiasing, true);// 反走样

    painter.setPen(QPen(QColor(0, 160, 230), 2));
    painter.setBrush(QColor(255, 160, 90));
    painter.drawEllipse(QPoint(labels[0].rx(), labels[0].ry()), 4, 4);

    painter.setPen(QPen(Qt::gray, 2));
    painter.setBrush(Qt::darkGreen);
    painter.drawEllipse(QPoint(labels[1].rx(), labels[1].ry()), 4, 4);

    painter.setPen(QPen(QColor(0, 160, 230), 1));
    painter.drawText(0, 0, 50, 10, Qt::AlignLeft, "measure");
    //painter.drawLine(showLine[0]);
    painter.drawLines(lines);

    painter.setPen(QPen(Qt::gray, 1));
    painter.drawText(0, 10, 50, 10, Qt::AlignLeft, "kalman");
    //painter.drawLine(showLine[1]);
    painter.drawLines(kalmanLines);
}

void MainWindow::handleTimeout() {
    qDebug() << QString("distCount:%0, POSITION(%1,%2,%3)->(%4,%5,%6) => d=%7").arg(distCount, 4)
                .arg(pos[distCount].x,4,'g',3).arg(pos[distCount].y,4,'g',3).arg(pos[distCount].z,4,'g',3)
                .arg(kalmanPos[distCount].x,4,'g',3).arg(kalmanPos[distCount].y,4,'g',3).arg(kalmanPos[distCount].z,4,'g',3)
                .arg(calcDistance(kalmanPos[distCount], pos[distCount]),4,'g',3);

    QPoint lastPoint = labels[0];
    labels[0].setX(int(pos[distCount].x));
    labels[0].setY(int(pos[distCount].y));
    showLine[0].setPoints(lastPoint, labels[0]);

    lastPoint = labels[1];
    labels[1].setX(int(kalmanPos[distCount].x));
    labels[1].setY(int(kalmanPos[distCount].y));
    showLine[1].setPoints(lastPoint, labels[1]);

    update();
    distCount++;

    if (distCount == dist.count())
        timer.stop();
}

void MainWindow::calcPosVector() {
    pos.append(calcFromDist(dist[0].distance));
    for (int i = 1; i < dist.count(); i++) {
        pos.append(calcFromDist(dist[i].distance));
        lines.append(QLine{int(pos[i-1].x), int(pos[i-1].y), int(pos[i].x), int(pos[i].y)});
    }
}
locationCoor MainWindow::calcFromDist(uint32_t dist[], uint32_t count) {
    QVector<locationCoor> Positions = calc.calcPosFromDistance(dist, count);
    locationCoor optimizedPos;
    double maxDistance = 0.0f;
    double tmpDistance = 0.0f;
    int idx = -1;
    locationCoor center = {0.0f, 0.0f, 0.0f};

    for (int loop = 0; loop < 1; loop++) {
        for (int i = 0; i < Positions.count(); i++) {
            center = center + Positions[i] / Positions.count();
        }
        for (int i = 0; i < Positions.count(); i++) {
            tmpDistance = calcDistance(center, Positions[i]);
            if (maxDistance < tmpDistance) {
                maxDistance = tmpDistance;
                idx = i;
            }
        }
        Positions.removeAt(idx);
        center = {0.0f, 0.0f, 0.0f};
        maxDistance = 0.0f;
    }

    for (int i = 0; i < Positions.count(); i++) {
        center = center + Positions[i] / Positions.count();
    }

    optimizedPos = center;
    return optimizedPos;
}
double MainWindow::calcTotalDistanceMeas() {
    double ans = 0.0f;
    for(int i = 10; i < lines.count(); i++) {
        ans += calcDistance(lines[i].p1(), lines[i].p2());
    }
    return ans/(lines.count()-10.0f);
}

void MainWindow::calcKalmanPosVector(double Q_in) {
    kalmanPos.clear();
    locationCoor x_hat_t;
    locationCoor x_t_1;
    locationCoor v_t_1;
    locationCoor v_t_2;
    locationCoor a_t_1;
    double delta_t = 1.f;
    locationCoor x_t_meas;
    locationCoor v_t_meas;
    double sigma_square_A_t_1;  //上一点的可信度，越可信，值越小
    double sigma_square_A_t;    //上一点的可信度，越可信，值越小
    double sigma_square_B;      //测量点的可信度，越可信，值越小
    double k;
    double Q = Q_in;
    //double R;   // is sigmaB2 itself
    locationCoor x_t;

    x_t_1 = pos[0];
    v_t_1 = {0,0,0};
    v_t_2 = {0,0,0};
    a_t_1 = {0,0,0};
    sigma_square_A_t_1 = 0.3f;
    kalmanPos.append(pos[0]);

    for(int i = 1; i < pos.count(); i++) {
        //x_hat_t = x_t_1 + (v_t_1 + a_t_1*delta_t/2.0f) * delta_t;
        x_hat_t = x_t_1 + v_t_1 * delta_t;
        x_t_meas = pos[i];
        v_t_meas = (x_t_meas - x_t_1) / delta_t;
        sigma_square_A_t = delta_t*delta_t * sigma_square_A_t_1 + Q;
        sigma_square_B = kalmanCalc::calcSigmaB(v_t_meas, v_t_1);
        k = sigma_square_A_t / (sigma_square_A_t + sigma_square_B);
        x_t = x_hat_t * (1 - k) + x_t_meas * k;
        kalmanPos.append(x_t);
        kalmanLines.append(QLine{int(kalmanPos[i-1].x), int(kalmanPos[i-1].y), int(kalmanPos[i].x), int(kalmanPos[i].y)});

        /*
        qDebug() << QString("i=%0, sigmaA2_t_1=%1, sigmaA2_t=%2, sigmaB2=%3, k=%4, dist_x=%5, dist_v=%6, Q=%7")
                    .arg(i, 4, 10, QChar('0'))
                    .arg(sigma_square_A_t_1, 8, 'g', 4)
                    .arg(sigma_square_A_t, 8, 'g', 4)
                    .arg(sigma_square_B, 8, 'g', 4)
                    .arg(k, 7, 'g', 4)
                    .arg(calcDistance(kalmanPos[i], pos[i]), 6, 'g', 3)
                    .arg(calcDistance(v_t_1, v_t_meas), 6, 'g', 3)
                    .arg(Q, 6, 'g', 3);
        */

        //a_t_1 = ((x_t - x_t_1) / delta_t - v_t_2) / (2*delta_t);
        v_t_2 = v_t_1;
        v_t_1 = (x_t - x_t_1) / delta_t;
        x_t_1 = x_t;
        a_t_1 = (v_t_1 - v_t_2) / delta_t;
        sigma_square_A_t_1 = sigma_square_A_t * (1 - k);
    }
}

double MainWindow::calcTotalDistanceKalman() {
    double ans = 0.0f;
    for(int i = 10; i < kalmanLines.count(); i++) {
        ans += calcDistance(kalmanLines[i].p1(), kalmanLines[i].p2());
    }
    return ans/(kalmanLines.count()-10.0f);
}
