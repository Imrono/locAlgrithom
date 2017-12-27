#include "uiMainWindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include "calcKalman.h"

uiMainWindow::uiMainWindow(showStore *store, QWidget *parent) :
    store(store), QMainWindow(parent), timerStarted(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(countChanged(int)), ui->canvas, SLOT(followMainWindowCount(int)));
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
        if (!ui->canvas->reverseShowPath()) {
            ui->showPath->setText(MY_STR("显示路径"));
        } else {
            ui->showPath->setText(MY_STR("隐藏路径"));
            ui->canvas->setLines(MEASUR_STR, this->store->getLabel(MEASUR_STR)->AnsLines);
            ui->canvas->setLines(KALMAN_STR, this->store->getLabel(KALMAN_STR)->AnsLines);
        }

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
        if (!ui->canvas->reverseShowAllPos()) {
            ui->allPos->setText(MY_STR("显示所有点"));
        } else {
            ui->allPos->setText(MY_STR("隐藏所有点"));
        }

        update();
    });
}

uiMainWindow::~uiMainWindow()
{
    delete ui;
}

void uiMainWindow::setConfigData(const configData *d) {
    ui->canvas->setConfigData(d);
}

void uiMainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画canvas的边框
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(ui->canvas->x() - 1, ui->canvas->y() - 1,
                     ui->canvas->x() + ui->canvas->width() + 1, ui->canvas->y() - 1);
    painter.drawLine(ui->canvas->x() - 1, ui->canvas->y() - 1,
                     ui->canvas->x() - 1, ui->canvas->y() + ui->canvas->height() + 1);
    painter.drawLine(ui->canvas->x() + ui->canvas->width() + 1, ui->canvas->y() + ui->canvas->height()+1,
                     ui->canvas->x() + ui->canvas->width() + 1, ui->canvas->y() - 1);
    painter.drawLine(ui->canvas->x() + ui->canvas->width() + 1, ui->canvas->y() + ui->canvas->height()+1,
                     ui->canvas->x() - 1, ui->canvas->y() + ui->canvas->height() + 1);

}

void uiMainWindow::handleTimeout() {
    distCount = distCount < 0 ? 0 : distCount;
    distCount++;    //为了保证qDebug与paintEvent显示一致，先distCount++，实际从1开始。
    emit countChanged(distCount);

    labelInfo *meas = store->getLabel(MEASUR_STR);
    labelInfo *kalm = store->getLabel(KALMAN_STR);

    locationCoor p_meas = meas->Ans[distCount];
    locationCoor p_kalm = kalm->Ans[distCount];
    qDebug() << QString("distCount:%0, POSITION(%1,%2,%3)->KALMAN(%4,%5,%6) => d=%7, totDist=%8, R=%9, P=%10"
                        ", K=%12")
                .arg(distCount, 4, 10, QChar('0'))
                .arg(p_meas.x,4,'g',3).arg(p_meas.y,4,'g',3).arg(p_meas.z,4,'g',3)
                .arg(p_kalm.x,4,'g',3).arg(p_kalm.y,4,'g',3).arg(p_kalm.z,4,'g',3)
                .arg(calcDistance(p_meas, p_kalm),4,'g',3)
                .arg(meas->Reliability[distCount],5,'g',3)
                .arg(kalm->data_R[distCount],5,'g',3)
                .arg(kalm->data_P[distCount],5,'g',3)
                .arg(kalm->Reliability[distCount],5,'g',3);

    ui->canvas->setPosition(MEASUR_STR, meas->Ans[distCount].toQPointF());
    ui->canvas->setLine(MEASUR_STR, meas->AnsLines[distCount-1]);
    ui->canvas->setPosition(KALMAN_STR, kalm->Ans[distCount].toQPointF());
    ui->canvas->setLine(KALMAN_STR, kalm->AnsLines[distCount-1]);

    ui->canvas->setPointsRaw(MEASUR_STR, meas->RawPoints[distCount]);
    ui->canvas->setPointsRefined(MEASUR_STR, meas->RefinedPoints[distCount]);

    //ui->raw_0->setText(QString::number(meas->));

    update();

    if (distCount == meas->Ans.count())
        timer.stop();
}
