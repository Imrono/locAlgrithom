#ifndef UI_CANVAS_H
#define UI_CANVAS_H
#include "_myheads.h"
#include "dataType.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"
#include "showTagRelated.h"

#include <QWidget>
#include <QDebug>

class uiCanvas : public QWidget
{
    Q_OBJECT
public:
    uiCanvas(QWidget *parent = 0);

    void setConfigData(const configData *cfg_d);
    void setDistanceData(const distanceData *dist_q);

    void setPosition(int tagIdx, const QString &methodName, const QPointF &p) {
        tags[tagIdx].setPosition(methodName, p);
    }
    void setLine(int tagIdx, const QString &methodName, const QLineF &l) {
        tags[tagIdx].setLine(methodName, l);
    }
    void setPointsRaw(int tagIdx, const QString &methodName, const QVector<locationCoor> ps) {
        tags[tagIdx].setPointsRaw(methodName, ps);
    }
    void setPointsRefined(int tagIdx, const QString &methodName, const QVector<locationCoor> ps) {
        tags[tagIdx].setPointsRefined(methodName, ps);
    }
    void setLines(int tagIdx, const QString &methodName, const QVector<QLineF> &lines) {
        tags[tagIdx].setLines(methodName, lines);
    }
    void setDistance(int tagIdx,  const int *dist) {
        QVector<int> d;
        for (int i = 0; i < cfg_d->sensor.count(); i++)
            d.append(dist[i]);
        tags[tagIdx].setDistance(d);
    }

    bool reverseShowPath() {
        isShowPath = !isShowPath;
        return isShowPath;
    }
    bool reverseShowAllPos() {
        isShowAllPos = !isShowAllPos;
        return isShowAllPos;
    }
    bool reverseShowRadius() {
        isShowRadius = !isShowRadius;
        return isShowRadius;
    }
    bool reverseShowTrack() {
        isShowTrack = !isShowTrack;
        return isShowTrack;
    }

    void loadPicture(QString path);

public slots:
    void followMainWindowCount(int cnt);

private:
    int widthCanvasOld{0};
    int heightCanvasOld{0};

    dType widthActual{4000.f};
    dType heightActual{3000.f};

    const configData   *cfg_d {nullptr};
    const distanceData *dist_d{nullptr};

    bool isShowSensor{true};
    QVector<QPointF> sensorShow;

    bool isShowStop{true};
    QVector<QPolygonF> stopShow;

    bool isShowAlarm{true};
    QVector<QPolygonF> alarmShow;

    bool isShowOper{true};
    QVector<QPolygonF> operShow;

    bool isShowRadius{false};
    bool isShowTrack{false};

    dType ratioShow{1.f};

    QMap<int, showTagRelated> tags;
    bool isShowPath{false};
    bool isShowAllPos{false};
    int nCount{0};

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    QImage backgroundImg;

    QPointF actual2Show(const locationCoor &p) {
        return p.toQPointF() * ratioShow;
    }
    void cfg_actualData2showData();
};

#endif // UI_CANVAS_H
