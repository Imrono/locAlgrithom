#ifndef UI_CANVAS_H
#define UI_CANVAS_H
#include "_myheads.h"
#include <QWidget>

#include "dataType.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"
#include "showTagRelated.h"
#include "uiUsrFrame.h"

class uiCanvas : public QWidget
{
    Q_OBJECT
public:
    uiCanvas(QWidget *parent = 0);

    void setConfigData(const configData *cfg_d);
    void setDistanceData(const distanceData *dist_q);
    void syncWithUiFrame(uiUsrFrame *frm);

    void setPosition(int tagId, const QString &methodName, const QPointF &p) {
        if (tags.contains(tagId)) {
            tags[tagId].setPosition(methodName, p);
        }
    }
    void setLine(int tagId, const QString &methodName, const QLineF &l) {
        if (tags.contains(tagId)) {
            tags[tagId].setLine(methodName, l);
        }
    }
    void setPointsRaw(int tagId, const QString &methodName, const QVector<locationCoor> ps) {
        if (tags.contains(tagId)) {
            tags[tagId].setPointsRaw(methodName, ps);
        }
    }
    void setPointsRefined(int tagId, const QString &methodName, const QVector<locationCoor> ps) {
        if (tags.contains(tagId)) {
            tags[tagId].setPointsRefined(methodName, ps);
        }
    }
    void setLines(int tagId, const QString &methodName, const QVector<QLineF> &lines) {
        if (tags.contains(tagId)) {
            tags[tagId].setLines(methodName, lines);
        }
    }
    void setDistance(int tagId,  const int *dist) {
        if (tags.contains(tagId)) {
            QVector<int> d;
            for (int i = 0; i < cfg_d->sensor.count(); i++)
                d.append(dist[i]);
            tags[tagId].setDistance(d);
        }
    }
    void clearData(int tagId) {
        if (tags.contains(tagId)) {
            tags[tagId].clearData();
        }
    }
    void removeAll() {
        tags.clear();
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
