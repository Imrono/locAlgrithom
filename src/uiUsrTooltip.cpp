#include "uiUsrTooltip.h"
#include "ui_usrtooltip.h"
#include <QPainter>

uiUsrTooltip::uiUsrTooltip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::usrtooltip)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "count");
    ui->qwtPlot->setAxisMaxMajor(QwtPlot::xBottom, 7);
    ui->qwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 2);
    ui->qwtPlot->setAxisMaxMajor(QwtPlot::yLeft, 4);
    ui->qwtPlot->setAxisMaxMinor(QwtPlot::yLeft, 5);
    //ui->qwtPlot->setAxisTitle(QwtPlot::yLeft,"value");
    //ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 10000, 1);
    //ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 500, 1);

    curve_v = new QwtPlotCurve("v");
    curve_v->setPen(Qt::yellow, 1);

    curve_a = new QwtPlotCurve("a");
    curve_a->setPen(Qt::green, 1);
}

uiUsrTooltip::~uiUsrTooltip()
{
    delete ui;

    if (curve_a)    delete curve_a;
    if (curve_v)    delete curve_v;
}

void uiUsrTooltip::setChartData(const QString &name,
                                const QVector<qreal> &v, const QVector<qreal> &a) {
    ui->qwtPlot->setTitle(name);

    QVector<qreal> x_v;
    QVector<qreal> tmpV = v;
    for(int i{0}; i < v.count(); i++) {
        x_v.append(i);
        qreal currV = v[i];
        if (currV > 10) tmpV[i] = 10;
        else            tmpV[i] = currV;
    }
    QVector<qreal> x_a;
    QVector<qreal> tmpA = a;
    for(int i{0}; i < a.count(); i++) {
        x_a.append(i);
        qreal currA = a[i];
        if (currA > 100)tmpA[i] = 100;
        else            tmpA[i] = currA;
    }

    curve_v->setSamples(x_v, tmpV);
    curve_v->attach(ui->qwtPlot);

    curve_a->setSamples(x_a, tmpA);
    //curve_a->attach(ui->qwtPlot);

    ui->qwtPlot->replot();
}

void uiUsrTooltip::fillToolTipUI(const tooltipInfo &info) {
    SET_NAN_TOOLTIP(6);
    SET_NAN_TOOLTIP(5);
    SET_NAN_TOOLTIP(4);
    SET_NAN_TOOLTIP(3);
    SET_NAN_TOOLTIP(2);
    SET_NAN_TOOLTIP(1);
    ui->labelPos->setText("");
    ui->labelTrack->setText("");
    ui->labelTagId->setText("");

    if (!info.isShow) {
        return;
    }

    ui->labelPos->setText(QString("pos: (%1, %2)").arg(info.pos.x()).arg(info.pos.y()));
    ui->labelTrack->setText(QString("track: (%1, %2)").arg(info.track.x()).arg(info.track.y()));
    ui->labelTagId->setText(QString("<b>%1<\b>").arg(info.tagId));

    switch (info.distance.count()) {
    case 6: SET_DATA_TOOLTIP(6);
    case 5: SET_DATA_TOOLTIP(5);
    case 4: SET_DATA_TOOLTIP(4);
    case 3: SET_DATA_TOOLTIP(3);
    case 2: SET_DATA_TOOLTIP(2);
    case 1: SET_DATA_TOOLTIP(1);
    default:
        break;
    }
}

void uiUsrTooltip::paintEvent(QPaintEvent *)
{
    QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
    linear.setColorAt(0, QColor(247, 247, 250));
    linear.setColorAt(0.5, QColor(240, 242, 247));
    linear.setColorAt(1, QColor(233, 233, 242));
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(linear);
    static const QPointF points[4] =
    {
        QPointF(0, 0),
        QPointF(0, this->height()-4),
        QPointF(this->width()-4, this->height()-4),
        QPointF(this->width()-4, 0),
    };
    painter.drawPolygon(points, 4);
}
