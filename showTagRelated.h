#ifndef SHOWPOINT_H
#define SHOWPOINT_H
#include "_myheads.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include "datatype.h"

class showTagRelated
{
public:
    explicit showTagRelated() :
        pos(QPoint{0,0}), radius(0), line(0,0,0,0)
    {}
    explicit showTagRelated(int r, const QPen pen, const QBrush brush, const QPen penRaw, const QBrush brushRaw) :
        pos(QPoint{0,0}), line(0,0,0,0), radius(r), painterPen(pen), painterBrush(brush),
        painterPenRaw(penRaw), painterBrushRaw(brushRaw)
    {}
    explicit showTagRelated(int r, const QPen pen, const QBrush brush) :
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

    /******************************************************************/
    void setRadius(int r) {
        radius = r;
    }
    void setPosition(const QPointF &p) {
        pos.setX(p.x());
        pos.setY(p.y());
    }
    void setPosition(const int x, const int y) {
        pos.setX(x);
        pos.setY(y);
    }
    QPointF getPosition() const {
        return pos;
    }

    void setLine(const QPointF &p1, const QPointF &p2) {
        line.setP1(p1);
        line.setP2(p2);
    }
    void setLine(const QLineF &l) {
        line.setLine(l.p1().x(), l.p1().y(), l.p2().x(), l.p2().y());
    }
    void setLine(int p1X, int p1Y, int p2X, int p2Y) {
        line.setLine(p1X, p1Y, p2X, p2Y);
    }
    QLineF getLine() const {
        return line;
    }

    void setLines(const QVector<QLineF> &lines) {
        this->lines = lines;
    }
    QVector<QLineF> getLines() const {
        return lines;
    }

    void setPointsRaw(const QVector<QPointF> ps) {
        pointsRaw = ps;
    }
    void setPointsRaw(QVector<locationCoor> ps) {
        pointsRaw.clear();
        for (int i = 0; i < ps.count(); i++)
            pointsRaw.append(ps[i].toQPointF());
    }
    void setPointsRefined(const QVector<QPointF> ps) {
        pointsRefined = ps;
    }
    void setPointsRefined(QVector<locationCoor> ps) {
        pointsRefined.clear();
        for (int i = 0; i < ps.count(); i++)
            pointsRefined.append(ps[i].toQPointF());
    }
    void setDistance(QVector<int> dist) {
        distance.clear();
        for (int i = 0; i < dist.count(); i++)
            distance.append(dist[i]);
    }

    void drawPoint(QPainter &painter, dType ratio = 1.f) const;
    void drawPointsRaw(QPainter &painter, dType ratio = 1.f) const;
    void drawPointsRefined(QPainter &painter, dType ratio = 1.f) const;
    void drawLine(QPainter &painter, dType ratio = 1.f) const;
    void drawLines(QPainter &painter, dType ratio = 1.f) const;
    void drawCircle(QPainter &painter, const QVector<locationCoor> &sensor, dType ratio = 1.f) const;

    showTagRelated& operator=(const showTagRelated &sp)
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
    QPointF pos;
    int     radius;
    QLineF  line;
    QVector<QPointF> pointsRaw;
    QVector<QPointF> pointsRefined;
    QVector<QLineF>  lines;

    QVector<int>     distance;

    QPen   painterPen;
    QBrush painterBrush;

    QPen   painterPenRaw;
    QBrush painterBrushRaw;
};

#endif // SHOWPOINT_H
