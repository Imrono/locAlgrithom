#ifndef DATADISTANCELOG_H
#define DATADISTANCELOG_H
#include "dataInputBase.h"
#include "_myheads.h"
#include "dataType.h"
#include <QVector>
#include <QLine>

struct distanceData {
    QMap<int, oneTag> tagsData; // tag's distance
    bool isInitialized{false};  // data ready
    void clear() {
        tagsData.clear();
        isInitialized = false;
    }
};

class dataInputLog : public dataInputBase
{
public:
    explicit dataInputLog();
    ~dataInputLog();

    void loadNewFile(const int type, const QString &fileName);
    void loadNewFile_1(const QString &fileName);
    void analyzeDistanceData1(const QStringList &strList, const QRegExp &rx);
    void loadNewFile_2(const QString &fileName);
    void analyzeDistanceData2(const QStringList &strList, const QRegExp &rx);

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

    int maxDataCount{0};

private:
    QString fileName;
    QRegExp rx;

    distanceData *q{nullptr};
};

#endif // DATADISTANCELOG_H
