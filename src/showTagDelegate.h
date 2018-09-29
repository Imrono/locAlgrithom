#ifndef SHOWPOINT_H
#define SHOWPOINT_H
#include "_myheads.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include "dataType.h"
#include "showTagColor.h"

struct showTagOneMethod {
    QString name;

    // 单次数据
    QPointF pos;            // 画坐标点
    QColor posColor;
    SHOW_SHAPE shape;

    int radius{4};
    QLineF line;            // 画上次坐标与本次坐标的连线

    // 轨迹数据
    QVector<QLineF> lines;
    QColor linesColor;

    // 只对测量数据有用
    QVector<QPointF> pointsRaw;
    QVector<QPointF> pointsRefined;
};

class showTagDelegate
{
public:
    explicit showTagDelegate();
    explicit showTagDelegate(int tagId);
    explicit showTagDelegate(int tagId, const oneTagView &tagView);

/******************************************************************/
    void addMethod(const QString &name);
    oneTagView getTagView() { return tagView;}

/******************************************************************/
    void setPosition(const QString &name, const QPointF &p) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].pos = p;
    }
    void setIterPos(const QVector<QPointF> &p) {
        iterPoints = p;
    }
    void setLine(const QString &name, const QLineF &l) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].line = l;
    }
    void setLines(const QString &name, const QVector<QLineF> &lines) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].lines = lines;
    }

    void setPointsRaw(const QString &name, const QVector<QPointF> &ps) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].pointsRaw = ps;
    }
    void setPointsRaw(const QString &name, const QVector<locationCoor> ps) {
        QVector<QPointF> tmp;
        for (int i = 0; i < ps.count(); i++)
            tmp.append(ps[i].toQPointF());
        setPointsRaw(name, tmp);
    }
    void setPointsRefined(const QString &name, const QVector<QPointF> &ps) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].pointsRefined = ps;
    }
    void setPointsRefined(const QString &name, const QVector<locationCoor> &ps) {
        QVector<QPointF> tmp;
        for (int i = 0; i < ps.count(); i++)
            tmp.append(ps[i].toQPointF());
        setPointsRefined(name, tmp);
    }

    void setDistance(const QVector<int> &dist, const QVector<bool> &used) {
        distance = dist;
        usedSensor = used;
    }

    void setWeight(const QVector<dType> &w) {
        weight = w;
    }

    void setGaussPoint(bool isShowGauss, QPointF x_hat) {
        isGaussPointAdded = isShowGauss;
        this->x_hat = x_hat;
    }

    void clearData() {
        oneTagMethod.clear();
        //distance.clear();
        weight.clear();
    }
    void clearMethodData(const QString &methodName) {
        if (oneTagMethod.contains(methodName))
            oneTagMethod.remove(methodName);
    }

/******************************************************************/
    void drawPoint(QPainter &painter, dType ratio = 1.f,
                   dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawTagId(QPainter &painter, dType ratio = 1.f,
                   dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawPointInfo(QPainter &painter, QPointF p, dType ratio = 1.f,
                       dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawIterPoints(QPainter &painter, dType ratio = 1.f,
                        dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawPointsRaw(QPainter &painter, dType ratio = 1.f,
                       dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawPointsRefined(QPainter &painter, dType ratio = 1.f,
                           dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawLine(QPainter &painter, dType ratio = 1.f,
                  dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawLines(QPainter &painter, dType ratio = 1.f,
                   dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawCircle(QPainter &painter, const QVector<locationCoor> &sensor,
                    dType ratio = 1.f, dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawCircleBold(QPainter &painter, const locationCoor &sensor, int distIdx,
                        dType ratio = 1.f, dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawLM(QPainter &painter, const QVector<locationCoor> &sensor, int w, int h,
                dType ratio = 1.f, dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;
    void drawCrossPos(QPainter &painter, const QVector<locationCoor> &sensor,
                   dType ratio = 1.f, dType zoom = 1.f, QPointF offset = QPointF(0,0)) const;

    static QPointF toZoomedPoint(QPointF p, dType ratio, dType zoom, QPointF offset) {
        return p*ratio*zoom+(1-zoom)*offset;
    }

    //showTagRelated& operator=(const showTagRelated &sp);
    // static set Max Likehood sigma
    static void setSigmaLM(int sigma) {
        sigmaLM = sigma;
    }

    QPointF getOneTagMeasurePos() const {
        if (oneTagMethod.contains(MEASUR_STR)) {
            return oneTagMethod[MEASUR_STR].pos;
        } else {
            return QPointF(0, 0);
        }
    }

private:
    QMap<QString, showTagOneMethod> oneTagMethod;
    int shapeSize{4};
    int tagId{-1};

    // 画圆，直观显示坐标计算过程
    QVector<int>  distance;
    QVector<bool> usedSensor;
    QVector<dType> weight;

    bool isGaussPointAdded{false};
    QPointF x_hat;

    QVector<QPointF> iterPoints;

    // 画笔和画刷的颜色
    oneTagView tagView;

/******************************************************************/
private:
    // static Max Likehood sigma
    static dType sigmaLM;
public:

    static void drawCrossPos(QPainter &painter, const QPointF center, const QColor &color,
                             dType r = 2.f);
    static void draw5Star(QPainter &painter, const QPointF center, const QColor &color,
                          dType r = 6.f, dType rot = 0.f);
    static void drawTriangle(QPainter &painter, const QPointF center, const QColor &color,
                             dType r = 4, dType rot = 0.f);
};

#endif // SHOWPOINT_H
