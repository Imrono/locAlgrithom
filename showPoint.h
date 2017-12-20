#ifndef SHOWPOINT_H
#define SHOWPOINT_H
#include <QPainter>
#include <QColor>
#include <QDebug>
#include "datatype.h"

class showPoint
{
public:
    explicit showPoint() :
        pos(QPoint{0,0}), radius(0), line(0,0,0,0)
    {}
    explicit showPoint(int r, const QPen pen, const QBrush brush, const QPen penRaw, const QBrush brushRaw) :
        pos(QPoint{0,0}), line(0,0,0,0), radius(r), painterPen(pen), painterBrush(brush),
        painterPenRaw(penRaw), painterBrushRaw(brushRaw)
    {}
    explicit showPoint(int r, const QPen pen, const QBrush brush) :
        pos(QPoint{0,0}), line(0,0,0,0), radius(r), painterPen(pen), painterBrush(brush)
    {}

    void setPen(const QPen pen) {
        painterPen = pen;
    }
    QPen getPen() {
        return painterPen;
    }

    void setBrush(const QBrush brush) {
        painterBrush = brush;
    }
    QBrush getBrush() const {
        return painterBrush;
    }
    void setBrushRaw(const QBrush brush) {
        painterBrushRaw = brush;
    }
    QBrush getBrushRaw() const {
        return painterBrushRaw;
    }

    void setRadius(int r) {
        radius = r;
    }
    void setPosition(const QPoint &p) {
        pos.setX(p.x());
        pos.setY(p.y());
    }
    void setPosition(const int x, const int y) {
        pos.setX(x);
        pos.setY(y);
    }
    QPoint getPosition() const {
        return pos;
    }

    void setLine(const QPoint &p1, const QPoint &p2) {
        line.setP1(p1);
        line.setP2(p2);
    }
    void setLine(const QLine &l) {
        line.setLine(l.p1().x(), l.p1().y(), l.p2().x(), l.p2().y());
    }
    void setLine(int p1X, int p1Y, int p2X, int p2Y) {
        line.setLine(p1X, p1Y, p2X, p2Y);
    }
    QLine getLine() const {
        return line;
    }

    void setPointsRaw(const QVector<QPoint> ps) {
        pointsRaw = ps;
    }
    void setPointsRaw(QVector<locationCoor> ps) {
        pointsRaw.clear();
        for (int i = 0; i < ps.count(); i++)
            pointsRaw.append(ps[i].toQPoint());
    }

    void drawPoint(QPainter &painter) const;
    void drawPointsRaw(QPainter &painter) const;
    void drawLine(QPainter &painter) const;
    void drawLines(QPainter &painter, QVector<QLine> lines) const;

    showPoint& operator=(const showPoint &sp)
    {
        pos = sp.pos;
        radius = sp.radius;
        painterPen = sp.painterPen;
        painterBrush = sp.painterBrush;
        painterPenRaw = sp.painterPenRaw;
        painterBrushRaw = sp.painterBrushRaw;
        return *this;
    }

private:
    QPoint pos;
    int    radius;
    QLine  line;
    QVector<QPoint> pointsRaw;

    QPen   painterPen;
    QBrush painterBrush;

    QPen   painterPenRaw;
    QBrush painterBrushRaw;
};

#endif // SHOWPOINT_H
