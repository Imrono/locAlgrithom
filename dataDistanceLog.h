#ifndef DATADISTANCELOG_H
#define DATADISTANCELOG_H
#include "_myheads.h"
#include "dataType.h"
#include <QVector>

struct distanceData {
    QVector<labelDistance> dist;
    QVector<oneTag_1> tagsData_1;
    QVector<oneTag_2> tagsData_2;
    bool isInitialized{false};

    int findTagByIdx_1(int idx) {
        for (int i = 0; i < tagsData_1.count(); i++) {
            if (tagsData_1[i].tagIdx == idx) {
                return i;
            } else {}
        }
        oneTag_1 tag(idx);
        tagsData_1.append(tag);
        return tagsData_1.count()-1;
    }

    int findTagByIdx_2(int idx) {
        for (int i = 0; i < tagsData_2.count(); i++) {
            if (tagsData_2[i].tagIdx == idx) {
                return i;
            } else {}
        }
        oneTag_2 tag(idx);
        tagsData_2.append(tag);
        return tagsData_2.count()-1;
    }
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

private:
    QString fileName;
    QRegExp rx;

    distanceData *q{nullptr};
};

#endif // DATADISTANCELOG_H
