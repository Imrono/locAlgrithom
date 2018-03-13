#ifndef DATADISTANCELOG_H
#define DATADISTANCELOG_H
#include "_myheads.h"
#include "dataType.h"
#include <QVector>
#include <QLine>

struct oneLogData {     // distance data
    QDateTime time;
    QVector<int> distance;
    locationCoor p_t_1;
    locationCoor p_t;
    QLineF l_t;
    QString toString();
};

struct oneTag {
    oneTag(int tagId) : tagId{tagId} {}
    oneTag() {}

    int tagId;
    QVector<oneLogData> distData;
};

struct distanceData {
    QMap<int, oneTag> tagsData; // tag's distance
    bool isInitialized{false};  // data ready
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
