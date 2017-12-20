#include "showPoint.h"

void showPoint::drawPoint(QPainter &painter) const {
    painter.setPen(painterPen);
    painter.setBrush(painterBrush);
    painter.drawEllipse(pos, radius, radius);
}
void showPoint::drawPointsRaw(QPainter &painter) const {
    painter.setPen(painterPenRaw);
    painter.setBrush(painterBrushRaw);
    for (int i = 0; i < pointsRaw.count(); i++) {
        painter.drawEllipse(pointsRaw[i], 3, 3);
    }
}

void showPoint::drawLine(QPainter &painter) const {
    painter.setPen(painterPen);
    painter.drawLine(line);
}
void showPoint::drawLines(QPainter &painter, QVector<QLine> lines) const {
    painter.setPen(painterPen);
    for (int i = 0; i < lines.count(); i++)
        painter.drawLine(lines[i]);
}
