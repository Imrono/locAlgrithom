#ifndef SHOWPOINT_H
#define SHOWPOINT_H
#include "_myheads.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include "dataType.h"

struct oneTagView {
    QColor color[2];
    SHOW_SHAPE type[2];
    int nColorStyle;
    bool isUsed;
};

struct tagsView {
    tagsView();
    static QList<oneTagView> viewDatabase;
    static int count;
};

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

class showTagRelated
{
public:
    explicit showTagRelated();
    explicit showTagRelated(int tagId);

/******************************************************************/
    void addMethod(const QString &name);

/******************************************************************/
    void setPosition(const QString &name, const QPointF &p) {
        if (!oneTagMethod.contains(name)) {
            addMethod(name);
        }
        oneTagMethod[name].pos = p;
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

    void setDistance(QVector<int> dist) {
        distance.clear();
        for (int i = 0; i < dist.count(); i++)
            distance.append(dist[i]);
    }

    void clearData() {
        oneTagMethod.clear();
        distance.clear();
    }

/******************************************************************/
    void drawPoint(QPainter &painter, dType ratio = 1.f) const;
    void drawPointsRaw(QPainter &painter, dType ratio = 1.f) const;
    void drawPointsRefined(QPainter &painter, dType ratio = 1.f) const;
    void drawLine(QPainter &painter, dType ratio = 1.f) const;
    void drawLines(QPainter &painter, dType ratio = 1.f) const;
    void drawCircle(QPainter &painter, const QVector<locationCoor> &sensor, dType ratio = 1.f) const;

    //showTagRelated& operator=(const showTagRelated &sp);
    static void resetColorCount() {tagsViewDataBase.count = 0;}
    static void decreaseColorCount() {tagsViewDataBase.count --;}

private:
    QMap<QString, showTagOneMethod> oneTagMethod;
    int shapeSize{4};
    int tagId{-1};

    // 画圆，直观显示坐标计算过程
    QVector<int> distance;

    // 画笔和画刷的颜色
    oneTagView tagView;

/******************************************************************/
    static QMap<int, oneTagView> tagViewData;
    static tagsView              tagsViewDataBase;
    static void recordTagId(int tagId);
    static void eraseTagId(int tagId);
};

#endif // SHOWPOINT_H
