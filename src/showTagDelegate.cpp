﻿#include "showTagDelegate.h"
#include "calcLibMath.h"
#include "calcLibGeometry.h"
#include <QtMath>

dType showTagDelegate::sigmaLM = 0.f;

/******************************************************************/
showTagDelegate::showTagDelegate(){
    usedSensor.fill(false, MAX_SENSOR);
}
showTagDelegate::showTagDelegate(int tagId) : tagId{tagId} {
    qDebug() << "[@showTagRelated::showTagRelated] tagId:" << this->tagId;
}
showTagDelegate::showTagDelegate(int tagId, const oneTagView &tagView)
    : tagId{tagId}, tagView{tagView}
{
    qDebug() << "[@showTagRelated::showTagRelated] tagId:" << this->tagId
             << this->tagView.color[0] << this->tagView.color[1] << this->tagView.isUsed;
}

void showTagDelegate::addMethod(const QString &name) {
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

void showTagDelegate::drawPoint(QPainter &painter, dType ratio,
                               dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen(Qt::black, 1));
        painter.drawEllipse(toZoomedPoint(oneMethod.pos, ratio, zoom, offset),
                            shapeSize, shapeSize);
    }
}
void showTagDelegate::drawTagId(QPainter &painter, dType ratio,
                                dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen(Qt::black, 1));
        painter.drawText(toZoomedPoint(oneMethod.pos, ratio, zoom, offset) + QPoint(shapeSize+2, shapeSize),
                         QString("%1").arg(tagId));
    }
}

void showTagDelegate::drawPointInfo(QPainter &painter, QPointF p, dType ratio,
                                    dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        QPointF diffPos = p - oneMethod.pos;
        if (QPointF::dotProduct(diffPos, diffPos) < MY_EPS) {
            painter.setPen  (QPen(QColor(Qt::darkGray), 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(toZoomedPoint(oneMethod.pos, ratio, zoom, offset),
                                MACRO_circleR_1*ratio*zoom, MACRO_circleR_1*ratio*zoom);
            painter.drawEllipse(toZoomedPoint(oneMethod.pos, ratio, zoom, offset),
                                MACRO_circleR_2*ratio*zoom, MACRO_circleR_2*ratio*zoom);
        } else {}
    }
}
void showTagDelegate::drawIterPoints(QPainter &painter, dType ratio,
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
void showTagDelegate::drawPointsRaw(QPainter &painter, dType ratio,
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
void showTagDelegate::drawPointsRefined(QPainter &painter, dType ratio,
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
void showTagDelegate::drawLine(QPainter &painter, dType ratio,
                              dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen  (QPen(oneMethod.posColor, 2));
        painter.drawLine(QLineF{toZoomedPoint(oneMethod.line.p1(), ratio, zoom, offset),
                                toZoomedPoint(oneMethod.line.p2(), ratio, zoom, offset)});
    }
}
void showTagDelegate::drawLines(QPainter &painter, dType ratio,
                               dType zoom, QPointF offset) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen(QPen(oneMethod.linesColor, 2));
        int lineCount = oneMethod.lines.count();
        for (int j = 1; j < lineCount; j++) {
            painter.drawLine(QLineF{toZoomedPoint(oneMethod.lines[j].p1(), ratio, zoom, offset),
                                    toZoomedPoint(oneMethod.lines[j].p2(), ratio, zoom, offset)});
            // qDebug() << "[@showTagRelated::drawLines]"
            //          << j << toZoomedPoint(oneMethod.lines[j].p1(), ratio, zoom, offset)
            //               << toZoomedPoint(oneMethod.lines[j].p2(), ratio, zoom, offset);
        }
    }
}

void showTagDelegate::drawCircle(QPainter &painter, const QVector<locationCoor> &sensor,
                                dType ratio, dType zoom, QPointF offset) const {
    int maxDistIdx = sensor.count() - 1;
    if (distance.count() > maxDistIdx && usedSensor.count() > maxDistIdx) {
        QPen tmpPen = QPen(Qt::darkGray, 1);
        //QPen tmpPen = QPen(Qt::black, 1);
        painter.setBrush(Qt::NoBrush);
        for (int i = 0; i < sensor.count(); i++) {
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
}

void showTagDelegate::drawCircleBold(QPainter &painter, const locationCoor &sensor,
                                    int distIdx, dType ratio, dType zoom, QPointF offset) const {
    if (distance.count() > distIdx && usedSensor.count() > distIdx) {
        QPen tmpPen = QPen(Qt::darkGray, 3);
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
}

void showTagDelegate::drawCrossPos(QPainter &painter, const QVector<locationCoor> &sensor,
                                   dType ratio, dType zoom, QPointF offset) const {
    dType x0_1, x0_2, y0_1, y0_2;
    bool isCross;
    for (int i = 0; i < sensor.count(); i++) {
        for (int j = i+1; j < sensor.count(); j++) {
            dType x1 = sensor[i].x;
            dType y1 = sensor[i].y;
            dType d1 = distance[i];
            dType x2 = sensor[j].x;
            dType y2 = sensor[j].y;
            dType d2 = distance[j];
            bool ans = calcCross2Circle(x1, y1, d1, x2, y2, d2,
                                        x0_1, y0_1, x0_2, y0_2, isCross);

            if (ans) {
                if (isCross) {
                    draw5Star(painter, toZoomedPoint(QPointF(x0_1, y0_1), ratio, zoom, offset),
                              tagView.color[0]);
                    draw5Star(painter, toZoomedPoint(QPointF(x0_2, y0_2), ratio, zoom, offset),
                              tagView.color[0]);
                } else {
                    //drawCross(painter, toZoomedPoint(QPointF(x0_1, y0_1), ratio, zoom, offset),
                    //          QColor(Qt::black), 3.f);
                    drawTriangle(painter, toZoomedPoint(QPointF(x0_1, y0_1), ratio, zoom, offset),
                                 QColor(Qt::black));
                }
            }
        }
    }
}

void showTagDelegate::drawLM(QPainter &painter, const QVector<locationCoor> &sensor, int w, int h,
                            dType ratio, dType zoom, QPointF offset) const {
    QVector<QPointF> zoomedSensor;
    QVector<double> zoomedDistance;
    QVector<double> zoomedWeight;

    for (int i = 0; i < sensor.count(); i++) {
        if (usedSensor[i]) {
            zoomedDistance.append(distance[i] * ratio * zoom);
            zoomedSensor.append(toZoomedPoint(sensor[i].toQPointF(), ratio, zoom, offset));
            zoomedWeight.append(weight[i]);
        }
        //qDebug() << i << "drawLM" << weight[i] << usedSensor[i];
    }
    if (isGaussPointAdded) {
        zoomedWeight.append(weight[sensor.count()]);
    } else {}

    double **matrix = new double*[w];
    for (int i = 0; i < w; i++) {
        matrix[i] = new double[h];
    }
    int div = 2;
    double tmpMax = 0.;
    int zoomedCount = zoomedSensor.count();
    for (int i = 0; i < w/div; i++) {
        for (int j = 0; j < h/div; j++) {
            QPointF p = QPointF(div*i, div*j);
            double z = 1.;
            for (int k = 0; k < zoomedCount; k++) {
                z *= normalDistribution(p, zoomedSensor[k], zoomedDistance[k],
                                        sigmaLM*ratio*zoom / zoomedWeight[k]) * 100.;
            }
            // add single point with gauss distribute
            if (isGaussPointAdded) {
                QPointF zoomedX_hat = toZoomedPoint(x_hat, ratio, zoom, offset);
                z *= normalDistribution(p, zoomedX_hat, 0. * ratio * zoom,
                                        sigmaLM*ratio*zoom / zoomedWeight[zoomedCount]) * 100.;
            }
            matrix[i][j] = z;
            tmpMax = matrix[i][j] > tmpMax ? matrix[i][j] : tmpMax;
        }
    }
    // black background
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(0, 0, w, h);
    for (int i = 0; i < w/div; i++) {
        for (int j = 0; j < h/div; j++) {
            int gray = static_cast<int>(255. * matrix[i][j] / tmpMax);
            if (gray > 5) {
                painter.setPen(QPen(QColor(gray, gray, gray), div));
                painter.drawPoint(div*i, div*j);
            }
        }
    }
    // clear matrix
    for (int i = 0; i < w; i++) {
        delete []matrix[i];
    }
    delete []matrix;
}

void showTagDelegate::drawCrossPos(QPainter &painter, const QPointF center, const QColor &color, dType r) {
    QPointF a1 = center + QPointF(-r, -r);
    QPointF b1 = center + QPointF(r, r);
    QPointF a2 = center + QPointF(r, -r);
    QPointF b2 = center + QPointF(-r, r);

    QPen tmpPen = painter.pen();
    painter.setPen(QPen(color));
    painter.drawLine(a1, b1);
    painter.drawLine(a2, b2);
    painter.setPen(tmpPen);
}
void showTagDelegate::draw5Star(QPainter &painter, const QPointF center, const QColor &color,
                                dType r, dType rot) {
    painter.setBrush(QBrush(color));
    painter.setPen  (Qt::NoPen);
    const dType coef = float(M_PI / 180.);
    dType r2 = sin(18.f*coef)/sin(54.f*coef) * r;
    QPolygonF tmpPolygon;
    dType angle = -90.f - rot;
    QPainterPath path;
    for (int i = 0; i < 5; i++) {
        dType tmpAngle = angle - i * 72.f;
        dType tmpAngle2 = tmpAngle - 36.f;
        tmpPolygon << QPointF(r * cos(tmpAngle*coef), r * sin(tmpAngle*coef)) + center;
        tmpPolygon << QPointF(r2 * cos(tmpAngle2*coef), r2 * sin(tmpAngle2*coef)) + center;
    }
    path.addPolygon(tmpPolygon);
    painter.drawPath(path);
}
void showTagDelegate::drawTriangle(QPainter &painter, const QPointF center, const QColor &color,
                                   dType r, dType rot) {
    painter.setBrush(QBrush(color));
    painter.setPen  (Qt::NoPen);
    const dType coef = float(M_PI / 180.);
    QPolygonF tmpPolygon;
    dType angle = -90.f - rot;
    QPainterPath path;
    for (int i = 0; i < 3; i++) {
        dType tmpAngle = angle - i * 120.f;
        tmpPolygon << QPointF(r * cos(tmpAngle*coef), r * sin(tmpAngle*coef)) + center;
    }
    path.addPolygon(tmpPolygon);
    painter.drawPath(path);
}

