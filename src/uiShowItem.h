#ifndef UISHOWITEM_H
#define UISHOWITEM_H
#include <QWidget>
#include <QRegion>
#include "_myheads.h"

class uiShowItem : public QWidget
{
public:
    uiShowItem(QWidget *parent = 0);

    enum ITEM_SHARP {
        CIRCLE,
        TRIANGLE,
        SQUARE,
        STAR_5,
    };

    void setRegion(ITEM_SHARP sharp) {
        this->sharp = sharp;
        setRegion();
    }
    void setRegion(ITEM_SHARP sharp, float r) {
        this->sharp = sharp;
        setSharpSize(r);
        setRegion();
    }
    void setRegion(ITEM_SHARP sharp, const QColor &color,
                   float r, float rot,
                   int lineWidth, const QColor &lineColor);
    void setRegion();

    void setSharpSize(float r);
    void setColor(const QColor &color, int lineWidth, const QColor &lineColor) {
        this->color = color;
        this->lineWidth = lineWidth;
        this->lineColor = lineColor;
    }

private:
    float R{20.f};
    QRegion region;
    ITEM_SHARP sharp;
    float rot{0.f};
    QPoint center{0, 0};

    int lineWidth{0};
    QColor lineColor;
    QColor color;

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // UISHOWITEM_H
