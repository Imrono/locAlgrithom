#ifndef SHOWSTORE_H
#define SHOWSTORE_H
#include <QVector>
#include "showPoint.h"

struct labelInfo {
    QVector<QPoint>          Ans;
    QVector<QVector<QPoint>> RawPoints;
    showPoint                showStyle;
    QString                  name;
};

class showStore
{
public:
    showStore();

    void appendLabel() {
        labelInfo label;
        labels.append(label);
    }
    void appendLabel(const labelInfo label) {
        labels.append(label);
    }

    void setName(int idx, QString name) {
        if (idx < labels.count())
            labels[idx].name = name;
    }
    void setShowStyle(int idx, QString showStyle) {
        if (idx < labels.count())
            labels[idx].showStyle = showStyle;
    }
    void setShowStyle(QString name, QString showStyle) {
        foreach (labelInfo &label, label) {
            if(label.name == name)
                label.showStyle = showStyle;
        }
    }

    void addRawPoints(int idx, QVector<QPoint> points) {
        if (idx < labels.count())
            labels[idx].RawPoints.append(points);
    }
    void addRawPoints(QString name, QVector<QPoint> points) {
        foreach (labelInfo &label, label) {
            if(label.name == name)
                label.RawPoints.append(points);
        }
    }
    void addAnsPoint(int idx, QPoint p) {
        if (idx < labels.count())
            labels[idx].Ans = p;
    }
    void addAnsPoint(QString name, QPoint p) {
        foreach (labelInfo &label, label) {
            if(label.name == name)
                label.Ans = p;
        }
    }

private:
    QVector<labelInfo> labels;
};

#endif // SHOWSTORE_H
