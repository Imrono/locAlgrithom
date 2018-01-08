#include "uiCanvas.h"
#include <QPainter>
#include <QPainterPath>

uiCanvas::uiCanvas(QWidget *parent) : QWidget(parent)
{
    widthCanvasOld = width();
    heightCanvasOld = height();
    ratioShow = static_cast<dType>(width()) / widthActual;
}

void uiCanvas::cfg_actualData2showData() {
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
    cfg_actualData2showData();

    update();
}
void uiCanvas::setDistanceData(const distanceData *dist_q) {
    this->dist_d = dist_q;
}

void uiCanvas::syncWithUiFrame(uiUsrFrame *frm) {
    QList<int> showableTags = frm->getShowableTags();
    for (auto it = tags.begin(); it != tags.end();) {
        if (showableTags.contains(it.key())) {
            ++it;
        } else {
            qDebug() << "[@uiCanvas::syncWithUiFrame] erase showTagRelated tagId:" << it.key();
            it = tags.erase(it);
            showTagRelated::decreaseColorCount();
        }
    }

    foreach (int tagId, showableTags) {
        if (!tags.contains(tagId)) {
            qDebug() << "[@uiCanvas::syncWithUiFrame] insert showTagRelated tagId:" << tagId;
            tags.insert(tagId, showTagRelated{tagId});
        }
    }

    qDebug() << "[@uiCanvas::syncWithUiFrame] current tags count:" << tags.count();
}

void uiCanvas::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    qDebug() << this->width() << this->height();

    if (widthCanvasOld != width() || heightCanvasOld != height()) {
        ratioShow = static_cast<dType>(width()) / widthActual;
        cfg_actualData2showData();
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
    //painter.setPen(tags[MEASUR_STR].getPen());
    //painter.drawText(5, 5, 50, 10, Qt::AlignLeft, MEASUR_STR);

    //painter.setPen(tags[KALMAN_STR].getPen());
    //painter.drawText(5, 15, 50, 10, Qt::AlignLeft, KALMAN_STR);

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

    // 画背景
    if (!backgroundImg.isNull()) {
        QRect rect(0,0,this->width(),this->height());
        painter.drawImage(rect, backgroundImg);
    }

    // [Sensor]
    if (isShowSensor) {
        painter.drawText(0, 100, 50, 10, Qt::AlignLeft, QString("%0").arg(this->width()));
        painter.setBrush(QColor(0, 0, 0));
        for (int i = 0; i < this->cfg_d->sensor.count(); i++) {
            painter.setPen(QPen(QColor(0, 0, 0), 2));
            painter.drawEllipse(sensorShow[i], 6, 6);
            // index the anchor
            painter.setPen(Qt::white);
            QFont font;
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(sensorShow[i].rx()-6, sensorShow[i].ry()-6, 12, 12, Qt::AlignCenter, QString::number(i, 10));
        }
    }

    // real time Point at TOP
    if (isShowPath) {
        foreach(const showTagRelated &tag, tags) {
            tag.drawLines(painter, ratioShow);
        }
    }

    foreach (showTagRelated tag, tags) {
        if (isShowRadius) {
            tag.drawCircle(painter, cfg_d->sensor, ratioShow);
        }
        tag.drawPoint(painter, ratioShow);
        tag.drawLine(painter, ratioShow);

        if (isShowTrack) {
        }

        if (isShowAllPos) {
            tag.drawPointsRaw(painter, ratioShow);
            tag.drawPointsRefined(painter, ratioShow);
        }
    }
}

void uiCanvas::loadPicture(QString path) {
    backgroundImg.load(path);

    // 白色变透明
    backgroundImg = backgroundImg.convertToFormat(QImage::Format_ARGB32);
    union myrgb
    {
        uint rgba;
        uchar rgba_bits[4];
    };
    myrgb* mybits =(myrgb*) backgroundImg.bits();
    int len = backgroundImg.width()*backgroundImg.height();
    while(len-- > 0)
    {
        // 要是白色(255, 255, 255)则改为透明色
        mybits->rgba_bits[3] = (mybits->rgba == 0xFFFFFFFF) ? 0 : 255;
        mybits++;
    }

    update();
}
