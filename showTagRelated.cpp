#include "showTagRelated.h"

void showTagRelated::drawPoint(QPainter &painter, dType ratio) const {
    painter.setPen(painterPen);
    painter.setBrush(painterBrush);
    painter.drawEllipse(pos*ratio, radius, radius);
}
void showTagRelated::drawPointsRaw(QPainter &painter, dType ratio) const {
    painter.setPen(painterPenRaw);
    painter.setBrush(painterBrushRaw);
    for (int i = 0; i < pointsRaw.count(); i++) {
        painter.drawEllipse(pointsRaw[i]*ratio, 3, 3);
    }
}
void showTagRelated::drawPointsRefined(QPainter &painter, dType ratio) const {
    painter.setPen(painterPenRaw);
    painter.setBrush(QBrush(Qt::black));
    for (int i = 0; i < pointsRefined.count(); i++) {
        painter.drawEllipse(pointsRefined[i]*ratio, 3, 3);
    }
}
void showTagRelated::drawLine(QPainter &painter, dType ratio) const {
    painter.setPen(painterPen);
    painter.drawLine(QLineF{line.p1()*ratio, line.p2()*ratio});
}
void showTagRelated::drawLines(QPainter &painter, dType ratio) const {
    painter.setPen(painterPen);
    for (int i = 0; i < lines.count(); i++)
        painter.drawLine(QLineF{lines[i].p1()*ratio, lines[i].p2()*ratio});
}

void showTagRelated::drawCircle(QPainter &painter, const QVector<locationCoor> &sensor, dType ratio) const {
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < distance.count(); i++) {
        painter.drawEllipse(sensor[i].toQPointF() * ratio, distance[i] * ratio, distance[i] * ratio);
    }
}
