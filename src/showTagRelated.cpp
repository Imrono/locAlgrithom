﻿#include "showTagRelated.h"

QList<oneTagView> tagsView::viewDatabase;
int tagsView::count = 0;
QMap<int, oneTagView> showTagRelated::tagViewData;
tagsView showTagRelated::tagsViewDataBase;

tagsView::tagsView(){
    viewDatabase << oneTagView{{QColor{Qt::green},  QColor{Qt::darkGreen}},  {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 0, false};
    viewDatabase << oneTagView{{QColor{Qt::red},    QColor{Qt::darkRed}},    {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 1, false};
    viewDatabase << oneTagView{{QColor{Qt::blue},   QColor{Qt::darkBlue}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 2, false};
    viewDatabase << oneTagView{{QColor{Qt::cyan},   QColor{Qt::darkCyan}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 3, false};
    viewDatabase << oneTagView{{QColor{Qt::magenta},QColor{Qt::darkMagenta}},{SHOW_SHAPE::radius, SHOW_SHAPE::square}, 4, false};
    viewDatabase << oneTagView{{QColor{Qt::yellow}, QColor{Qt::darkYellow}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 5, false};
    count = 0;
}

/******************************************************************/
showTagRelated::showTagRelated(){}
showTagRelated::showTagRelated(int tagId) : tagId{tagId} {
    recordTagId(tagId);
    tagView = tagViewData[tagId];
    qDebug() << "[@showTagRelated::showTagRelated] tagId:" << tagId << tagView.color[0] << tagView.color[1] << tagView.isUsed;
}
void showTagRelated::recordTagId(int tagId) {
    if (!tagViewData.contains(tagId)) {
        for (int i = 0; i < tagsViewDataBase.viewDatabase.count(); i++) {
            if (false == tagsViewDataBase.viewDatabase[i].isUsed) {
                tagsViewDataBase.viewDatabase[i].isUsed = true;
                tagViewData.insert(tagId, tagsViewDataBase.viewDatabase[i]);
                tagsViewDataBase.count++;
                break;
            }
        }
    }
}
void showTagRelated::eraseTagId(int tagId) {
    if (tagViewData.contains(tagId)) {
        tagsViewDataBase.viewDatabase[tagViewData[tagId].nColorStyle].isUsed = false;
        tagViewData.remove(tagId);
        tagsViewDataBase.count--;
    }
}

void showTagRelated::addMethod(const QString &name) {
    if (!oneTagMethod.contains(name)) {
        showTagOneMethod tmp;
        tmp.name = name;
        tmp.posColor = tagView.color[oneTagMethod.count()%2];
        tmp.linesColor = tagView.color[oneTagMethod.count()%2];
        oneTagMethod.insert(name, tmp);
    } else {
        qWarning() << "showTagRelated::addMethod $>" << name << "is Already exist";
    }
}
void showTagRelated::setTagView() {
    if (tagViewData.contains(tagId)) {
        tagView = tagViewData[tagId];
    }
}

void showTagRelated::drawPoint(QPainter &painter, dType ratio,
                               dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen  (QColor(Qt::darkGray), 2));
        //painter.drawEllipse(oneMethod.pos*ratio, shapeSize, shapeSize);
        painter.drawEllipse(toZoomedPoint(oneMethod.pos, ratio, zoom, offset),
                            shapeSize, shapeSize);
        //qDebug() << oneMethod.pos*ratio-offset << offset;
    }
}
void showTagRelated::drawIterPoints(QPainter &painter, dType ratio,
                                    dType zoom, QPointF offset) const {
    painter.setBrush(QBrush(QColor(Qt::white)));
    QFont font;
    font.setBold(false);
    painter.setFont(font);
    for (int i = 0; i < iterPoints.count(); i++) {
        QPointF p = toZoomedPoint(iterPoints[i], ratio, zoom, offset);
        painter.setPen(QPen(QColor(Qt::white), 0));
        painter.drawEllipse(p, shapeSize*2, shapeSize*2);
        painter.setPen(Qt::black);
        painter.drawText(p.x()-5, p.y()-5, 10, 10, Qt::AlignCenter, QString::number(i));
        //qDebug() << "iteration" << i << iterPoints[i];
    }
}
void showTagRelated::drawPointsRaw(QPainter &painter, dType ratio,
                                   dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen  (QColor(Qt::darkGray), 1));
        for (int j = 0; j < oneMethod.pointsRaw.count(); j++) {
            painter.drawEllipse(toZoomedPoint(oneMethod.pointsRaw[j], ratio, zoom, offset),
                                shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawPointsRefined(QPainter &painter, dType ratio,
                                       dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(QColor(Qt::darkGray)));
        painter.setPen  (QPen  (Qt::black, 1));
        for (int j = 0; j < oneMethod.pointsRefined.count(); j++) {
            painter.drawEllipse(toZoomedPoint(oneMethod.pointsRefined[j], ratio, zoom, offset),
                                shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawLine(QPainter &painter, dType ratio,
                              dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen  (QPen(oneMethod.posColor, 2));
        painter.drawLine(QLineF{toZoomedPoint(oneMethod.line.p1(), ratio, zoom, offset),
                                toZoomedPoint(oneMethod.line.p2(), ratio, zoom, offset)});
    }
}
void showTagRelated::drawLines(QPainter &painter, dType ratio,
                               dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen(QPen(oneMethod.linesColor, 2));
        for (int j = 0; j < oneMethod.lines.count(); j++)
            painter.drawLine(QLineF{toZoomedPoint(oneMethod.lines[j].p1(), ratio, zoom, offset),
                                    toZoomedPoint(oneMethod.lines[j].p2(), ratio, zoom, offset)});
    }
}

void showTagRelated::drawCircle(QPainter &painter, const QVector<locationCoor> &sensor,
                                dType ratio, dType zoom, QPointF offset) const {
    QPen tmpPen = QPen(Qt::black, 1);
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < distance.count(); i++) {
        if (usedSensor[i]) {
            tmpPen.setStyle(Qt::SolidLine);
        } else {
            tmpPen.setStyle(Qt::DotLine);
        }
        painter.setPen(tmpPen);
        painter.drawEllipse(toZoomedPoint(sensor[i].toQPointF(), ratio, zoom, offset),
                            distance[i] * ratio * zoom, distance[i] * ratio * zoom);
    }
}

void showTagRelated::drawCircleBold(QPainter &painter, const locationCoor &sensor,
                                    int distIdx, dType ratio, dType zoom, QPointF offset) const {
    QPen tmpPen = QPen(Qt::black, 3);
    painter.setBrush(Qt::NoBrush);
    if (usedSensor[distIdx]) {
        tmpPen.setStyle(Qt::SolidLine);
    } else {
        tmpPen.setStyle(Qt::DotLine);
    }
    painter.setPen(tmpPen);

    painter.drawEllipse(toZoomedPoint(sensor.toQPointF(), ratio, zoom, offset),
                        distance[distIdx] * ratio * zoom, distance[distIdx] * ratio * zoom);
}