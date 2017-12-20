#ifndef SHOWSTORE_H
#define SHOWSTORE_H
#include <QVector>
#include <QMap>
#include "showPoint.h"
#include "datatype.h"

#define MEASUR_STR "measure"
#define KALMAN_STR "kalman"

struct labelInfo {
    QVector<locationCoor>          Ans;
    QVector<double>                Reliability; //1.meas：长度之和；2.kalman：卡尔曼增益K
    QVector<double>                dataR;
    QVector<double>                dataP;
    QVector<QLine>                 AnsLines;
    QVector<QVector<locationCoor>> RawPoints;
    showPoint                      showStyle;
    QString                        name;
    QString toString() {
        return QString("name:%0, nLines:%1, nAnsPoints:%2")
                .arg(name, 8).arg(AnsLines.count(), 4).arg(Ans.count(), 4);
    }
};

class showStore
{
public:
    showStore(locationCoor loc[4]);

    QMap<QString, int> name2idx;
    locationCoor loc[4];

    void appendLabel(const QString &name);
    void appendLabel(const QString &name, const showPoint &showStyle);
    void appendLabel(labelInfo *label);
    labelInfo * getLabel(int idx);
    labelInfo * getLabel(const QString &name) {
        return getLabel(name2idx[name]);
    }

    void setName(int idx, const QString &name);
    void setShowStyle(int idx, showPoint showStyle);

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
