#ifndef UI_CANVAS_H
#define UI_CANVAS_H
#include "_myheads.h"
#include "datatype.h"
#include "dataSensorIni.h"
#include "showTagRelated.h"

#include <QWidget>
#include <QDebug>

class uiCanvas : public QWidget
{
    Q_OBJECT
public:
    uiCanvas(QWidget *parent = 0);

    void setConfigData(const configData *cfg_d);

    void setPosition(QString name, const QPointF &p) {
        tags[name].setPosition(p);
    }
    void setLine(QString name, const QLineF &l) {
        tags[name].setLine(l);
    }
    void setPointsRaw(QString name, const QVector<locationCoor> ps) {
        tags[name].setPointsRaw(ps);
    }
    void setPointsRefined(QString name, const QVector<locationCoor> ps) {
        tags[name].setPointsRefined(ps);
    }
   void setLines(QString name, const QVector<QLineF> &lines) {
        tags[name].setLines(lines);
    }

    bool reverseShowPath() {
        isShowPath = !isShowPath;
        return isShowPath;
    }
    bool reverseShowAllPos() {
        isShowAllPos = !isShowAllPos;
        return isShowAllPos;
    }

public slots:
    void followMainWindowCount(int cnt);

private:
    int widthCanvasOld{0};
    int heightCanvasOld{0};

    dType widthActual{4000.f};
    dType heightActual{3000.f};

    const configData *cfg_d{nullptr};

    bool isShowSensor{true};
    QVector<QPointF> sensorShow;

    bool isShowStop{true};
    QVector<QPolygonF> stopShow;

    bool isShowAlarm{true};
    QVector<QPolygonF> alarmShow;

    bool isShowOper{true};
    QVector<QPolygonF> operShow;

    dType ratioShow{1.f};

    QMap<QString, showTagRelated> tags;
    bool isShowPath{false};
    bool isShowAllPos{false};
    int nCount{0};

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    QPointF actual2Show(const locationCoor &p) {
        return QPointF(p.x * ratioShow, p.y * ratioShow);
    }
    void actualData2showData();
};

#endif // UI_CANVAS_H
