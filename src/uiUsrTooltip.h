#ifndef USRTOOLTIP_H
#define USRTOOLTIP_H

#include <QWidget>
#include "_myheads.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>

namespace Ui {
class usrtooltip;
}

#define SET_NAN_TOOLTIP(n) \
    ui->diff_##n->setText("NaN");\
    ui->dist_##n->setText("NaN");\
    ui->w_##n->setText("NaN");

#define SET_DATA_TOOLTIP(n) \
    ui->diff_##n->setText(QString("%1").arg(info.diff[n-1], 5).left(6));\
    ui->dist_##n->setText(QString("%1").arg(info.distance[n-1]));\
    ui->w_##n->setText(QString("%1").arg(info.weight[n-1], 5).left(6));

struct tooltipInfo {
    QVector<int>   distance;
    QVector<dType> weight;
    QVector<dType>   diff;
    int count;
    int tagId;
    QPointF pos;
    QPointF track;
    bool isShow;
};

class uiUsrTooltip : public QWidget
{
    Q_OBJECT

public:
    explicit uiUsrTooltip(QWidget *parent = 0);
    ~uiUsrTooltip();

    void fillToolTipUI(const tooltipInfo &info);
    void setChartData(const QString &name,
                      const QVector<qreal> &v, const QVector<qreal> &a);

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::usrtooltip *ui;

    QwtPlotCurve *curve_v;
    QwtPlotCurve *curve_a;
};

#endif // USRTOOLTIP_H
