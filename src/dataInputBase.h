#ifndef DATACALCANALYZEINPUTBASE_H
#define DATACALCANALYZEINPUTBASE_H
#include <QObject>
#include <QLineF>
#include "_myheads.h"

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

class uiMainWindow;
class dataInputBase : public QObject
{
    Q_OBJECT
public:
    dataInputBase();

    void setMainWin(uiMainWindow *mainWin) {
        this->mainWin = mainWin;
    }
    void setSensorNum(int n) {
        sensorNum =n;
    }

protected:
    uiMainWindow *mainWin;
    int sensorNum{6};
};

#endif // DATACALCANALYZEINPUTBASE_H
