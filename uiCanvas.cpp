#include "uiCanvas.h"
#include <QPainter>
#include <QPainterPath>

uiCanvas::uiCanvas(QWidget *parent) : QWidget(parent)
{
    tags.insert(MEASUR_STR, showTagRelated {5, QPen(Qt::blue , 2), QBrush(Qt::magenta),
                                               QPen(Qt::black, 1), QBrush(Qt::magenta)});
    tags.insert(KALMAN_STR, showTagRelated {5, QPen(Qt::gray , 2), QBrush(Qt::darkGreen)});

    widthCanvasOld = width();
    heightCanvasOld = height();
    ratioShow = static_cast<dType>(width()) / widthActual;
}

void uiCanvas::followMainWindowCount(int cnt) {
    nCount = cnt;
}

void uiCanvas::actualData2showData() {
    if (nullptr == cfg_d) {
        qDebug() << "d == nullptr";
        return;
    }

    qDebug() << "ratioShow:" << ratioShow;

    // [Sensor]
    sensorShow.clear();
    for (int i = 0; i < cfg_d->sensor.count(); i++) {
        sensorShow.append(actual2Show(cfg_d->sensor[i]));
        qDebug() << "d->sensor[" << i << "]," << this->cfg_d->sensor[i].toString()
                 << "sensorShow[" << i << "], (" << sensorShow[i].x() << "," << sensorShow[i].y() << ")";
    }
    // [Stop]
    stopShow.clear();
    for (int i = 0; i < cfg_d->stop.count(); i++) {
        QPolygonF tmpPolygon;
        for (int j = 0; j < cfg_d->stop[i].count(); j++) {
            tmpPolygon << actual2Show(cfg_d->stop[i][j]);
        }
        stopShow.append(tmpPolygon);
    }
    // [Alarm]
    alarmShow.clear();
    for (int i = 0; i < cfg_d->alarm.count(); i++) {
        QPolygonF tmpPolygon;
        for (int j = 0; j < cfg_d->alarm[i].count(); j++) {
            tmpPolygon << actual2Show(cfg_d->alarm[i][j]);
        }
        alarmShow.append(tmpPolygon);
    }
    // [Oper]
    operShow.clear();
    for (int i = 0; i < cfg_d->oper.count(); i++) {
        QPolygonF tmpPolygon;
        for (int j = 0; j < cfg_d->oper[i].count(); j++) {
            tmpPolygon << actual2Show(cfg_d->oper[i][j]);
        }
        operShow.append(tmpPolygon);
    }
}

void uiCanvas::setConfigData(const configData *d) {
    if (nullptr == d)
        return;

    this->cfg_d = d;
    actualData2showData();

    update();
}

void uiCanvas::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    qDebug() << this->width() << this->height();

    if (widthCanvasOld != width() || heightCanvasOld != height()) {
        ratioShow = static_cast<dType>(width()) / widthActual;
        actualData2showData();
    }
    widthCanvasOld = width();
    heightCanvasOld = height();

    update();
}

void uiCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画canvas的边框
    painter.setPen(QPen(Qt::gray, 5));
    painter.drawRect(geometry());
    // legend
    painter.setPen(tags[MEASUR_STR].getPen());
    painter.drawText(0, 0, 50, 10, Qt::AlignLeft, MEASUR_STR);

    painter.setPen(tags[KALMAN_STR].getPen());
    painter.drawText(0, 12, 50, 10, Qt::AlignLeft, KALMAN_STR);
    // count
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(0, 24, 50, 10, Qt::AlignLeft, QString("%0").arg(nCount, 4, 10, QChar('0')));

    painter.setPen(QPen(Qt::black, 0, Qt::NoPen));
    // [Alarm]
    painter.setBrush(QBrush(Qt::yellow));
    QPainterPath alarmPath;
    for (int i = 0; i < alarmShow.count(); i++) {
        alarmPath.addPolygon(alarmShow[i]);
    }
    painter.drawPath(alarmPath);

    // [Stop]
    painter.setBrush(QBrush(Qt::red));
    QPainterPath stopPath;
    for (int i = 0; i < stopShow.count(); i++) {
        stopPath.addPolygon(stopShow[i]);
    }
    painter.drawPath(stopPath);

    // [Oper]
    painter.setBrush(QBrush(Qt::green));
    QPainterPath operPath;
    for (int i = 0; i < operShow.count(); i++) {
        operPath.addPolygon(operShow[i]);
    }
    painter.drawPath(operPath);

    // [Sensor]
    if (isShowSensor) {
        painter.drawText(0, 100, 50, 10, Qt::AlignLeft, QString("%0").arg(this->width()));
        painter.setBrush(QColor(0, 0, 0));
        for (int i = 0; i < this->cfg_d->sensor.count(); i++) {
            painter.setPen(QPen(QColor(0, 0, 0), 2));
            painter.drawEllipse(sensorShow[i], 5, 5);
            // index the anchor
            painter.setPen(Qt::blue);
            QFont font;
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(sensorShow[i].rx()-6, sensorShow[i].ry()-6, 12, 12, Qt::AlignCenter, QString::number(i, 10));
        }
    }

    // real time Point at TOP
    if (isShowPath) {
        tags[MEASUR_STR].drawLines(painter, ratioShow);
        tags[KALMAN_STR].drawLines(painter, ratioShow);
    }

    tags[MEASUR_STR].drawPoint(painter, ratioShow);
    tags[MEASUR_STR].drawLine(painter, ratioShow);
    tags[KALMAN_STR].drawPoint(painter, ratioShow);
    tags[KALMAN_STR].drawLine(painter, ratioShow);

    if (isShowAllPos) {
        tags[MEASUR_STR].drawPointsRaw(painter, ratioShow);
        tags[MEASUR_STR].drawPointsRefined(painter, ratioShow);
    }
}
