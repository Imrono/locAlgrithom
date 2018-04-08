#include "uiCanvas.h"
#include "calcLibGeometry.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

uiCanvas::uiCanvas(QWidget *parent) : QWidget(parent)
{
    widthCanvas = width();
    heightCanvas = height();
    ratioShow = static_cast<dType>(width()) / widthActual;
    setMouseTracking(true);

    item.move(10, 10);
    item.setRegion(uiShowItem::CIRCLE, 10.f);
    item.show();
}

void uiCanvas::cfg_actualData2showData() {
    if (nullptr == cfg_d) {
        qDebug() << "d == nullptr";
        return;
    }

    qDebug() << "[@uiCanvas::setConfigData] ratioShow:" << ratioShow;

    // [Sensor]
    sensorShow.clear();
    for (int i = 0; i < cfg_d->sensor.count(); i++) {
        sensorShow.append(actual2Show(cfg_d->sensor[i]));
        qDebug() << "[@uiCanvas::setConfigData] d->sensor[" << i << "]," << this->cfg_d->sensor[i].toString()
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

void uiCanvas::syncWithUiFrame(uiUsrFrame *frm) {
    QList<int> showableTags = frm->getShowableTags();
    for (auto it = tags.begin(); it != tags.end();) {
        // if tagId is enabled and is showing, discard it
        if (showableTags.contains(it.key())) {
            ++it;
        // if tagId is not enabled but is showing, stop showing
        } else {
            qDebug() << "[@uiCanvas::syncWithUiFrame] erase showTagRelated tagId:" << it.key();
            showTagDelegate::eraseTagId(it.key());
            it = tags.erase(it);
        }
    }

    foreach (int tagId, showableTags) {
        // if tagId is enabled but not showing, show it
        if (!tags.contains(tagId)) {
            qDebug() << "[@uiCanvas::syncWithUiFrame] insert showTagRelated tagId:" << tagId;
            tags.insert(tagId, showTagDelegate{tagId});
            showTagDelegate::recordTagId(tagId);
            tags[tagId].setTagView();
            frm->setBtnColorSample(tagId, tags[tagId].getTagView().color[0]);
        }
    }

    qDebug() << "[@uiCanvas::syncWithUiFrame] current tags count:" << tags.count();
}

void uiCanvas::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    qDebug() << "[@uiCanvas::resizeEvent]" << this->width() << this->height();

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

    // draw canvas background and frame
    painter.setPen(QPen(Qt::gray, 5));
    //painter.setBrush(Qt::white);
    painter.drawRect(geometry());

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

        // draw background
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

    foreach (const showTagDelegate tag, tags) {
        if (isShowAllPos) {
            tag.drawPointsRaw(painter, ratioShow, zoom(), center);
            tag.drawPointsRefined(painter, ratioShow, zoom(), center);
        }

        if (isShowLM) {
            tag.drawLM(painter, cfg_d->sensor, width(), height(),
                       ratioShow, zoom(), center);
        }

        if (isShowPath) {
            tag.drawLines(painter, ratioShow, zoom(), center);
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
        if (isShowCross) {
            tag.drawCrossPos(painter, cfg_d->sensor, ratioShow, zoom(), center);
        }

        tag.drawLine(painter, ratioShow, zoom(), center);
        tag.drawPoint(painter, ratioShow, zoom(), center);

        if (isShowPosInfo) {
            tag.drawPointInfo(painter, showPosInfo, ratioShow, zoom(), center);
        }
    }

    // [Sensor]
    if (isShowLM) {
        showSensors(painter);
    }

    if (isDrawClosestPos && isShowPath) {   // show closest point
        QPointF showPoint = showTagDelegate::toZoomedPoint(closestPos, ratioShow, zoom(), offset);
        showTagDelegate::drawCrossPos(painter, showPoint, Qt::black, 3.f);
        if (isDrawClosestInfo) {
            painter.drawText(showPoint+QPointF(5,-5), QString("tagId:%1").arg(closestPosTagId));
            painter.drawText(showPoint+QPointF(5, 5), QString("Count:%1").arg(closestPosCount));
        }
    }
}

void uiCanvas::mouseMoveEvent(QMouseEvent *event) {
    QPointF p = QPointF(event->x(), event->y());
    QPointF ans = ((p-center)/zoom()+center)/ratioShow;
    emit mouseChange(ans.x(), ans.y()); // show mouse pos at status bar (actural pos)
}

void uiCanvas::mousePressEvent(QMouseEvent *event) {
    QPointF mousePos = QPointF(event->x(), event->y());

    // show radius (distance from sensor)
    isShowRadiusBold = false;
    boldRadiusIdx = -1;
    for (int i = 0; i < cfg_d->sensor.count(); i++) {
        dType d = calcDistance(mousePos, toZoomedPoint(sensorShow[i]));
        if (d < 9) {
            isShowRadiusBold = true;
            boldRadiusIdx = i;
            break;
        }
    }

    // pos info show
    dType min_d = 10.f;
    foreach (const QPointF p, pos) {
        dType d = calcDistance(mousePos, toZoomedPoint(actual2Show(locationCoor(p))));
        if (d < 9) {
            isShowPosInfo = true;
            if (min_d > d) {
                showPosInfo = p;
                min_d = d;
            } else {}
        }
    }
    // qDebug() << "[@uiCanvas::mousePressEvent]" << showPosInfo << isShowPosInfo << mousePos << pos;

    if (isDrawClosestPos && isShowPath) {   // show closest point info
        isDrawClosestInfo = true;
    }

    update();
}
void uiCanvas::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    isShowRadiusBold = false;
    boldRadiusIdx = -1;
    isShowPosInfo = false;

    isDrawClosestInfo = false;

    update();
}

void uiCanvas::drawClosestPos(bool isDraw, int tagId, int n, QPointF p) {
    isDrawClosestPos = isDraw;
    closestPos = p;
    closestPosCount = n;
    closestPosTagId = tagId;
    update();
}

void uiCanvas::loadPicture(QString path) {
    backgroundImg.load(path);

    // white to transparent
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
        // if white, namely (255,255,255), change it to transparent
        mybits->rgba_bits[3] = (mybits->rgba == 0xFFFFFFFF) ? 0 : 255;
        mybits++;
    }
    backgroundImg.scaled(widthActual, heightActual);

    update();
}
