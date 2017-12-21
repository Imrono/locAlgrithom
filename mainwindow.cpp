#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include "kalmanCalc.h"

MainWindow::MainWindow(showStore *store, QWidget *parent) :
    store(store), QMainWindow(parent), timerStarted(false), isShowPath{false}, isShowAllPos{false},
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(ui->beginTrack, &QPushButton::clicked, this, [this](void) {
        if (timerStarted) {
            timer.stop();
            this->ui->beginTrack->setText("track");
        } else {
            timer.start(500);
            ui->beginTrack->setText("stop");
        }
        timerStarted = !timerStarted;
    });
    connect(ui->showPath, &QPushButton::clicked, this, [this](void) {
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        ui->showPath->setFont(font);
        if (isShowPath) {
            ui->showPath->setText("SHOW PATH");
        } else {
            ui->showPath->setText("HIDE PATH");
        }
        isShowPath = !isShowPath;
        update();
    });
    connect(ui->reset, &QPushButton::clicked, this, [this](void) {
        distCount = 0;
        handleTimeout();
    });
    connect(ui->previous, &QPushButton::clicked, this, [this](void) {
        distCount -= 2;     //handleTimeout()里有distCount++，所以这里-2
        handleTimeout();
    });
    connect(ui->next, &QPushButton::clicked, this, [this](void) {
        handleTimeout();
    });
    connect(ui->allPos, &QPushButton::clicked, this, [this](void) {
        if (isShowAllPos) {
            ui->allPos->setText("show pos");
        } else {
            ui->allPos->setText("hide pos");
        }
        isShowAllPos = !isShowAllPos;
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(401, 0, 401, 300);

    // 中间探测器所在区域
    painter.setPen(QPen(Qt::gray, 0));
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

    labelInfo *meas = store->getLabel(MEASUR_STR);
    labelInfo *kalm = store->getLabel(KALMAN_STR);

    painter.setPen(meas->showStyle.getPen());
    painter.drawText(0, 0, 50, 10, Qt::AlignLeft, "measure");
    //painter.drawLines(lines);

    painter.setPen(kalm->showStyle.getPen());
    painter.drawText(0, 12, 50, 10, Qt::AlignLeft, "kalman");
    //painter.drawLines(kalmanLines);

    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(0, 24, 50, 10, Qt::AlignLeft, QString("%0").arg(distCount, 4, 10, QChar('0')));
    //painter.drawLines(kalmanLines);

    if (isShowPath) {
        meas->showStyle.drawLines(painter, meas->AnsLines);
        kalm->showStyle.drawLines(painter, kalm->AnsLines);
    }

    meas->showStyle.drawPoint(painter);
    meas->showStyle.drawLine(painter);
    kalm->showStyle.drawPoint(painter);
    kalm->showStyle.drawLine(painter);

    if (isShowAllPos) {
        meas->showStyle.drawPointsRaw(painter);
        meas->showStyle.drawPointsRefined(painter);
    }
}

void MainWindow::handleTimeout() {
    distCount++;    //为了保证qDebug与paintEvent显示一致，先distCount++，实际从1开始。
    if (distCount <= 1)
        return;

    labelInfo *meas = store->getLabel(MEASUR_STR);
    labelInfo *kalm = store->getLabel(KALMAN_STR);

    locationCoor p_meas = meas->Ans[distCount];
    locationCoor p_kalm = kalm->Ans[distCount];
    qDebug() << QString("distCount:%0, POSITION(%1,%2,%3)->KALMAN(%4,%5,%6) => d=%7, totDist=%8, R=%9, P=%10, K=%11")
                .arg(distCount, 4, 10, QChar('0'))
                .arg(p_meas.x,4,'g',3).arg(p_meas.y,4,'g',3).arg(p_meas.z,4,'g',3)
                .arg(p_kalm.x,4,'g',3).arg(p_kalm.y,4,'g',3).arg(p_kalm.z,4,'g',3)
                .arg(calcDistance(p_meas, p_kalm),4,'g',3)
                .arg(meas->Reliability[distCount],5,'g',3)
                .arg(kalm->dataR[distCount],5,'g',3)
                .arg(kalm->dataP[distCount],5,'g',3)
                .arg(kalm->Reliability[distCount],5,'g',3);

    meas->showStyle.setPosition(meas->Ans[distCount].toQPoint());
    meas->showStyle.setLine(meas->AnsLines[distCount-1]);
    kalm->showStyle.setPosition(kalm->Ans[distCount].toQPoint());
    kalm->showStyle.setLine(kalm->AnsLines[distCount-1]);

    meas->showStyle.setPointsRaw(meas->RawPoints[distCount]);
    meas->showStyle.setPointsRefined(meas->RefinedPoints[distCount]);

    update();

    if (distCount == meas->Ans.count())
        timer.stop();
}
