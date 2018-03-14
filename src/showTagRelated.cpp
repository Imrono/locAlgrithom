#include "showTagRelated.h"
#include "calcLibMath.h"
#include "calcLibGeometry.h"
#include <QtMath>

dType showTagRelated::sigmaLM = 0.f;

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
        painter.drawEllipse(toZoomedPoint(oneMethod.pos, ratio, zoom, offset),
                            shapeSize, shapeSize);
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
		int lineCount = oneMethod.lines.count();
		for (int j = 0; j < lineCount; j++) {
			painter.drawLine(QLineF{toZoomedPoint(oneMethod.lines[j].p1(), ratio, zoom, offset),
									toZoomedPoint(oneMethod.lines[j].p2(), ratio, zoom, offset)});
			qDebug() << j << toZoomedPoint(oneMethod.lines[j].p1(), ratio, zoom, offset)
				          << toZoomedPoint(oneMethod.lines[j].p2(), ratio, zoom, offset);
		}
    }
}

void showTagRelated::drawCircle(QPainter &painter, const QVector<locationCoor> &sensor,
                                dType ratio, dType zoom, QPointF offset) const {
    QPen tmpPen = QPen(Qt::darkGray, 1);
    //QPen tmpPen = QPen(Qt::black, 1);
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

void showTagRelated::drawCross(QPainter &painter, const QVector<locationCoor> &sensor,
                               dType ratio, dType zoom, QPointF offset) const {
    dType x0_1, x0_2, y0_1, y0_2;
    bool isCross;
    for (int i = 0; i < distance.count(); i++) {
        for (int j = i+1; j < distance.count(); j++) {
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
                    drawCross(painter, toZoomedPoint(QPointF(x0_1, y0_1), ratio, zoom, offset),
                              tagView.color[0], 3.f);
                    drawCross(painter, toZoomedPoint(QPointF(x0_2, y0_2), ratio, zoom, offset),
                              tagView.color[0], 3.f);
                } else {
                    drawCross(painter, toZoomedPoint(QPointF(x0_1, y0_1), ratio, zoom, offset),
                              QColor(Qt::black), 3.f);
                }
            }
        }
    }
}

void showTagRelated::drawLM(QPainter &painter, const QVector<locationCoor> &sensor, int w, int h,
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

void showTagRelated::drawCross(QPainter &painter, const QPointF center, const QColor &color, dType r) {
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
