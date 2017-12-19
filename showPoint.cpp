#include "showPoint.h"

void showPoint::drawPoint(QPainter &painter) {
    painter.setPen(painterPen);
    painter.setBrush(painterBrush);
    painter.drawEllipse(pos, radius, radius);
}
void showPoint::drawLine(QPainter &painter) {
    painter.setPen(painterPen);
    painter.drawLine(line);
}
void showPoint::drawLines(QPainter &painter, QVector<QLine> lines) {
    painter.setPen(painterPen);
    for (int i = 0; i < lines.count(); i++)
        painter.drawLine(lines[i]);
}
