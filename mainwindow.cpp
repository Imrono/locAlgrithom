#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include "kalmanCalc.h"

MainWindow::MainWindow(showStore *store, QWidget *parent) :
    store(store), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        QPoint p(store->loc[i].toQPoint());
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

    painter.setPen(store->getLabel(MEASUR_STR)->showStyle.getPen());
    painter.drawText(0, 0, 50, 10, Qt::AlignLeft, "measure");
    //painter.drawLines(lines);

    painter.setPen(store->getLabel(KALMAN_STR)->showStyle.getPen());
    painter.drawText(0, 10, 50, 10, Qt::AlignLeft, "kalman");
    //painter.drawLines(kalmanLines);

    store->getLabel(MEASUR_STR)->showStyle.drawPoint(painter);
    store->getLabel(MEASUR_STR)->showStyle.drawLine(painter);
    store->getLabel(KALMAN_STR)->showStyle.drawPoint(painter);
    store->getLabel(KALMAN_STR)->showStyle.drawLine(painter);

    store->getLabel(MEASUR_STR)->showStyle.drawLines(painter, store->getLabel(MEASUR_STR)->AnsLines);
    store->getLabel(KALMAN_STR)->showStyle.drawLines(painter, store->getLabel(KALMAN_STR)->AnsLines);
}

void MainWindow::handleTimeout() {
    locationCoor p_meas = store->getLabel(MEASUR_STR)->Ans[distCount];
    locationCoor p_kalm = store->getLabel(KALMAN_STR)->Ans[distCount];
    qDebug() << QString("distCount:%0, POSITION(%1,%2,%3)->KALMAN(%4,%5,%6) => d=%7").arg(distCount, 4)
                .arg(p_meas.x,4,'g',3).arg(p_meas.y,4,'g',3).arg(p_meas.z,4,'g',3)
                .arg(p_kalm.x,4,'g',3).arg(p_kalm.y,4,'g',3).arg(p_kalm.z,4,'g',3)
                .arg(calcDistance(p_meas, p_kalm),4,'g',3);

    store->getLabel(MEASUR_STR)->showStyle.setPosition(store->getLabel(MEASUR_STR)->Ans[distCount].toQPoint());
    store->getLabel(MEASUR_STR)->showStyle.setLine(store->getLabel(MEASUR_STR)->AnsLines[distCount-1]);
    store->getLabel(KALMAN_STR)->showStyle.setPosition(store->getLabel(KALMAN_STR)->Ans[distCount].toQPoint());
    store->getLabel(KALMAN_STR)->showStyle.setLine(store->getLabel(KALMAN_STR)->AnsLines[distCount-1]);

    update();
    distCount++;

    if (distCount == store->getLabel(MEASUR_STR)->Ans.count())
        timer.stop();
}
