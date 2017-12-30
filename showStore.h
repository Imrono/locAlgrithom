#ifndef SHOWSTORE_H
#define SHOWSTORE_H
#include "_myheads.h"
#include <QVector>
#include <QMap>
#include "showTagRelated.h"
#include "datatype.h"

struct labelInfo {
    QVector<locationCoor>          Ans;
    QVector<dType>                 Reliability; //1.meas：长度之和；2.kalman：卡尔曼增益K
    QVector<dType>                 data_R;
    QVector<dType>                 data_P;
    QVector<dType>                 data_y;
    QVector<QLineF>                AnsLines;
    QVector<QVector<locationCoor>> RawPoints;
    QVector<QVector<locationCoor>> RefinedPoints;
    showTagRelated                 showStyle;
    QString                        name;
    QString toString() {
        return QString("name:%0, nLines:%1, nAnsPoints:%2")
                .arg(name, 8).arg(AnsLines.count(), 4).arg(Ans.count(), 4);
    }
    void resetTrack() {
        Ans.clear();
        Reliability.clear();
        data_R.clear();
        data_P.clear();
        data_y.clear();
        AnsLines.clear();
    }
};

class showStore
{
public:
    showStore();

    QMap<QString, int> name2idx;

    void appendLabel(const QString &name);
    void appendLabel(const QString &name, const showTagRelated &showStyle);
    void appendLabel(labelInfo *label);
    labelInfo * getLabel(int idx);
    labelInfo * getLabel(const QString &name) {
        return getLabel(name2idx[name]);
    }

    void setName(int idx, const QString &name);
    void setShowStyle(int idx, showTagRelated showStyle);

    void addRawPoints(int idx, QVector<locationCoor> points);
    void addRawPoints(const QString &name, QVector<locationCoor> points) {
        addRawPoints(name2idx[name], points);
    }

    void clearRawPoints(int idx);

    void addAnsPoint(int idx, locationCoor p);
    void addAnsPoint(const QString &name, locationCoor p) {
        addAnsPoint(name2idx[name], p);
    }

    QVector<locationCoor> getAnsPointVector(int idx);
    QVector<locationCoor> getAnsPointVector(const QString &name) {
        return getAnsPointVector(name2idx[name]);
    }

    locationCoor getAnsPoint(int idx, int pointIdx);
    locationCoor getAnsPoint(const QString &name, int pointIdx) {
        return getAnsPoint(name2idx[name], pointIdx);
    }

    void clearAnsPoints(int idx);

private:
    QVector<labelInfo *> labels;
};

#endif // SHOWSTORE_H
