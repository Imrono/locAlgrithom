#include "uiCanvas.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

uiCanvas::uiCanvas(QWidget *parent) : QWidget(parent)
{
    // white background
    //QPalette pal(palette());
    //pal.setColor(QPalette::Background, Qt::white);
    //setAutoFillBackground(true);
    //setPalette(pal);

    widthCanvas = width();
    heightCanvas = height();
    ratioShow = static_cast<dType>(width()) / widthActual;
    setMouseTracking(true);
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
            frm->clrBtnColorA(it.key());
            showTagRelated::eraseTagId(it.key());
            it = tags.erase(it);
        }
    }

    foreach (int tagId, showableTags) {
        if (!tags.contains(tagId)) {
            qDebug() << "[@uiCanvas::syncWithUiFrame] insert showTagRelated tagId:" << tagId;
            tags.insert(tagId, showTagRelated{tagId});
            showTagRelated::recordTagId(tagId);
            tags[tagId].setTagView();
            frm->setBtnColorA(tagId, tags[tagId].getTagView().color[0]);
        }
    }

    qDebug() << "[@uiCanvas::syncWithUiFrame] current tags count:" << tags.count();
}

void uiCanvas::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    qDebug() << this->width() << this->height();

    if (widthCanvas != width() || heightCanvas != height()) {
        ratioShow = static_cast<dType>(width()) / widthActual;
        center = QPoint(width()/2, height()/2);
        cfg_actualData2showData();
    }
    widthCanvas = width();
    heightCanvas = height();

    update();
}

void uiCanvas::showSensors(QPainter &painter) {
    QFont font;
    font.setBold(true);
    painter.setFont(font);
    if (isShowSensor) {
        painter.setBrush(QColor(0, 0, 0));
        for (int i = 0; i < this->cfg_d->sensor.count(); i++) {
            painter.setPen(QPen(QColor(0, 0, 0), 2));
            painter.drawEllipse(toZoomedPoint(sensorShow[i]), 6, 6);
            // index the anchor
            painter.setPen(Qt::white);
            painter.drawText(toZoomedPoint(sensorShow[i]).rx()-6,
                             toZoomedPoint(sensorShow[i]).ry()-6,
                             12, 12, Qt::AlignCenter, QString::number(i, 10));
        }
    }
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

    if (!isShowLM) {
        // [Alarm]
        painter.setBrush(QBrush(Qt::yellow));
        QPainterPath alarmPath;
        for (int i = 0; i < alarmShow.count(); i++) {
            QPolygonF tmpAlarm;
            for (int j = 0; j < alarmShow[i].count(); j++) {
                tmpAlarm << toZoomedPoint(alarmShow[i][j]);
            }
            alarmPath.addPolygon(tmpAlarm);
        }
        painter.drawPath(alarmPath);

        // [Stop]
        painter.setBrush(QBrush(Qt::red));
        QPainterPath stopPath;
        for (int i = 0; i < stopShow.count(); i++) {
            QPolygonF tmpStop;
            for (int j = 0; j < stopShow[i].count(); j++) {
                tmpStop << toZoomedPoint(stopShow[i][j]);
            }
            stopPath.addPolygon(tmpStop);
        }
        painter.drawPath(stopPath);

        // [Oper]
        painter.setBrush(QBrush(Qt::green));
        QPainterPath operPath;
        for (int i = 0; i < operShow.count(); i++) {
            QPolygonF tmpOper;
            for (int j = 0; j < operShow[i].count(); j++) {
                tmpOper << toZoomedPoint(operShow[i][j]);
            }
            operPath.addPolygon(tmpOper);
        }
        painter.drawPath(operPath);

        // 画背景
        if (!backgroundImg.isNull()) {
            toZoomedPoint(QPointF(0.f, 0.f));
            QRect rect(toZoomedPoint(QPointF(0.f, 0.f)).x(),
                       toZoomedPoint(QPointF(0.f, 0.f)).y(),
                       width() * zoom(), height() * zoom());
            painter.drawImage(rect, backgroundImg);
        }
        // [Sensor]
        showSensors(painter);
    }

    // real time Point at TOP
    if (isShowPath) {
        foreach(const showTagRelated &tag, tags) {
            tag.drawLines(painter, ratioShow, zoom(), center);
        }
    }

    foreach (showTagRelated tag, tags) {
        if (isShowAllPos) {
            tag.drawPointsRaw(painter, ratioShow, zoom(), center);
            tag.drawPointsRefined(painter, ratioShow, zoom(), center);
        }

        if (isShowLM) {
            tag.drawLM(painter, cfg_d->sensor, width(), height(),
                       ratioShow, zoom(), center);
        }

        if (isShowTrace) {
            tag.drawIterPoints(painter, ratioShow, zoom(), center);
        }

        if (isShowRadius) {
            tag.drawCircle(painter, cfg_d->sensor, ratioShow, zoom(), center);
        }
        if (isShowRadiusBold) {
            tag.drawCircleBold(painter, cfg_d->sensor[boldRadiusIdx], boldRadiusIdx,
                               ratioShow, zoom(), center);
        }

        tag.drawLine(painter, ratioShow, zoom(), center);
        tag.drawPoint(painter, ratioShow, zoom(), center);
    }

    // [Sensor]
    if (isShowLM)
        showSensors(painter);
}

void uiCanvas::mouseMoveEvent(QMouseEvent *event) {
    QPointF p = QPointF(event->x(), event->y());
    QPointF ans = ((p-center)/zoom()+center)/ratioShow;
    emit mouseChange(ans.x(), ans.y());
}

void uiCanvas::mousePressEvent(QMouseEvent *event) {
    dType x = event->x();
    dType y = event->y();

    isShowRadiusBold = false;
    boldRadiusIdx = -1;
    for (int i = 0; i < cfg_d->sensor.count(); i++) {
        dType d = qSqrt(qPow(x - toZoomedPoint(sensorShow[i]).x(), 2)
                      + qPow(y - toZoomedPoint(sensorShow[i]).y(), 2));
        if (d < 9) {
            isShowRadiusBold = true;
            boldRadiusIdx = i;
            break;
        }
    }
    update();
}
void uiCanvas::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    isShowRadiusBold = false;
    boldRadiusIdx = -1;
    update();
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
    backgroundImg.scaled(widthActual, heightActual);

    update();
}
