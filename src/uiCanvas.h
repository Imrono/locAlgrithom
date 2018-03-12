#ifndef UI_CANVAS_H
#define UI_CANVAS_H
#include "_myheads.h"
#include <QWidget>
#include <QMouseEvent>
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
    void setIterPoints(int tagId, const QVector<QPointF> &p) {
        if (tags.contains(tagId)) {
            tags[tagId].setIterPos(p);
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
    void setDistance(int tagId, const int *dist, bool *usedSensor) {
        if (tags.contains(tagId)) {
            QVector<int> d;
            QVector<bool> used;
            for (int i = 0; i < cfg_d->sensor.count(); i++) {
                d.append(dist[i]);
                used.append(usedSensor[i]);
            }
            tags[tagId].setDistance(d, used);
        }
    }
    void setWeight(int tagId, const QVector<dType> &weight) {
        if (tags.contains(tagId)) {
            tags[tagId].setWeight(weight);
        }
    }
    void setGaussPoint(int tagId, bool isShowGauss, QPointF x_hat = QPointF(0.f, 0.f)) {
        if (tags.contains(tagId)) {
            tags[tagId].setGaussPoint(isShowGauss, x_hat);
        }
    }
    void clearData(int tagId) {
        if (tags.contains(tagId)) {
            tags[tagId].clearData();
        }
    }
    void removeAll() {
        for (auto it = tags.begin(); it != tags.end();) {
            qDebug() << "[@uiCanvas::removeAll] erase showTagRelated tagId:" << it.key();
            showTagRelated::eraseTagId(it.key());
            it = tags.erase(it);
        }
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
        isShowTrace = !isShowTrace;
        return isShowTrace;
    }
    bool reverseShowCross() {
        isShowCross = !isShowCross;
        return isShowCross;
    }

    void setShowLM(bool lm) {
        isShowLM = lm;
    }

    void loadPicture(QString path);
    void zoomChange(int ratio = 0) {
        if (ratioZoom + ratio > 0) {
            ratioZoom += ratio;
        }
        update();
    }
    void zoomReset() {ratioZoom = 100;}
    int zoomGet() {return ratioZoom;}
    dType zoom() {return ratioZoom/100.f;}

    dType widthActual{4000.f};
    dType heightActual{3000.f};

signals:
    void mouseChange(int x, int y);

private:
    int widthCanvas{0};
    int heightCanvas{0};

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
    bool isShowRadiusBold{false};
    int  boldRadiusIdx{-1};
    bool isShowTrace{false};
    bool isShowCross{false};

    dType ratioShow{1.f};
    QPoint center;
    int ratioZoom{100};
    QPoint offset{QPoint(0,0)};
    QPointF toZoomedPoint(QPointF p) {
        return p*zoom()+(1.f-zoom())*center;
    }

    QMap<int, showTagRelated> tags;
    bool isShowPath{false};
    bool isShowAllPos{false};
    bool isShowLM{false};

    int nCount{0};

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QImage backgroundImg;

    QPointF actual2Show(const locationCoor &p) {
        return p.toQPointF() * ratioShow;
    }
    void cfg_actualData2showData();

    void showSensors(QPainter &painter);
};

#endif // UI_CANVAS_H
