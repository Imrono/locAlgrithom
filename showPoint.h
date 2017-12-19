#ifndef SHOWPOINT_H
#define SHOWPOINT_H
#include <QPainter>
#include <QColor>

class showPoint
{
public:
    explicit showPoint() :
        pos(QPoint{0,0}), radius(0), line(0,0,0,0)
    {}
    explicit showPoint(int r, const QPen pen, const QBrush brush) :
        pos(QPoint{0,0}), line(0,0,0,0), radius(r), painterPen(pen), painterBrush(brush)
    {}
    explicit showPoint(QPoint p, QLine l, int r, const QPen pen, const QBrush brush) :
        pos(p), radius(r), line(l), painterPen(pen), painterBrush(brush)
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
    QPoint getPosition() {
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
    QLine getLine() {
        return line;
    }

    void drawPoint(QPainter &painter);
    void drawLine(QPainter &painter);
    void drawLines(QPainter &painter, QVector<QLine> lines);

    showPoint& operator=(const showPoint &sp)
    {
        pos = sp.pos;
        radius = sp.radius;
        painterPen = sp.painterPen;
        painterBrush = sp.painterBrush;
        return *this;
    }

private:
    QPoint pos;
    int    radius;
    QLine  line;

    QPen   painterPen;
    QBrush painterBrush;
};

#endif // SHOWPOINT_H
