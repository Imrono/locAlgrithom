#ifndef DATADISTANCELOG_H
#define DATADISTANCELOG_H
#include "_myheads.h"
#include "datatype.h"
#include <QVector>

struct distanceData {
    QVector<labelDistance> dist;
    //QVector<dType>         distAnalyized;
    //QString toString();
};

class dataDistanceLog
{
public:
    explicit dataDistanceLog();
    ~dataDistanceLog();
    void loadNewFile(const QString &fileName);

    QString toString();

    distanceData *get_q() const {
        return q;
    }

private:
    QString fileName;
    distanceData *q{nullptr};
};

#endif // DATADISTANCELOG_H
