#include "uiShowItem.h"
#include <QtMath>

uiShowItem::uiShowItem(QWidget *parent) : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    setSharpSize(R);
}

void uiShowItem::setRegion() {
    qDebug() << "[@uiShowItem::setRegion]" << R << sharp << rot
             << "width" << width() << "height" << height();
    if (STAR_5 == sharp) {
        const float coef = float(M_PI / 180.);
        float r1 = R;
        float r2 = sin(18.f*coef)/sin(54.f*coef) * R;
        QPolygon pa;
        float angle = -90.f - rot;
        for (int i = 0; i < 5; i++) {
            float tmpAngle1 = angle - i * 72.f;
            float tmpAngle2 = tmpAngle1 - 36.f;
            QPoint p1 = QPoint(r1 * cos(tmpAngle1*coef), r1 * sin(tmpAngle1*coef)) + center;
            QPoint p2 = QPoint(r2 * cos(tmpAngle2*coef), r2 * sin(tmpAngle2*coef)) + center;
            pa << p1 << p2;
        }
        region = QRegion(pa);
    } else if (TRIANGLE == sharp) {
        const dType coef = float(M_PI / 180.);
        QPolygon pa;
        dType angle = -90.f - rot;
        for (int i = 0; i < 3; i++) {
            dType tmpAngle = angle - i * 120.f;
            pa << QPoint(R * cos(tmpAngle*coef), R * sin(tmpAngle*coef)) + center;
        }
        region = QRegion(pa);
    } else if (CIRCLE == sharp) {
        region = QRegion(0, 0, R*2.f, R*2.f, QRegion::Ellipse);
    } else {}

    setMask(region);
}

void uiShowItem::enterEvent(QEvent *event) {
    QWidget::enterEvent(event);
}

void uiShowItem::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
}

void uiShowItem::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

void uiShowItem::setSharpSize(float r) {
    R = r;
    center = QPoint{int(R), int(R)};
    resize(R*2, R*2);
}

void uiShowItem::setRegion(ITEM_SHARP sharp, const QColor &color,
                           float r, float rot,
                           int lineWidth, const QColor &lineColor) {
    this->sharp = sharp;
    this->rot = rot;
    setSharpSize(r);
    setColor(color, lineWidth, lineColor);

    setRegion();
}
