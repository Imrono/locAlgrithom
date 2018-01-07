#ifndef DATADISTANCELOG_H
#define DATADISTANCELOG_H
#include "_myheads.h"
#include "dataType.h"
#include <QVector>

struct oneLogData {
    QDateTime time;
    QVector<int> distance;
    QString toString();
};

struct oneTag {
    oneTag(int tagId) : tagId{tagId} {}
    oneTag() {}

    int tagId;
    QVector<oneLogData> distData;
};

struct distanceData {
    //QVector<labelDistance> dist;
    QMap<int, oneTag> tagsData;
    bool isInitialized{false};
};

class dataDistanceLog
{
public:
    explicit dataDistanceLog();
    ~dataDistanceLog();
    void loadNewFile_1(const QString &fileName);
    void loadNewFile_2(const QString &fileName);

    QString toString();

    distanceData *get_q() const {
        return q;
    }
    void clear() {
        if (nullptr != q) {
            delete q;
            q = new distanceData;
        }
    }

    void composeMeasData();

    int maxDataCount{0};

private:
    QString fileName;
    QRegExp rx;

    distanceData *q{nullptr};
};

#endif // DATADISTANCELOG_H
