#ifndef SHOWSTORE_H
#define SHOWSTORE_H
#include "_myheads.h"
#include <QVector>
#include <QMap>
#include "showTagRelated.h"
#include "dataType.h"

struct storeTagInfo;
struct storeMethodInfo {
    storeMethodInfo() {}
    storeMethodInfo(const QString &name, storeTagInfo *parent) {
        methodName = name;
        parentTag  = parent;
    }

    storeTagInfo *parentTag{nullptr};
    QString methodName;
    bool isMethodActive{false};
    QVector<QDateTime>             time;
    QVector<locationCoor>          Ans;
    QVector<QLineF>                AnsLines;
    // data[0]
    // meas: Reliability; kalman: R
    QVector<dType>                 data[5];
    void clear() {
        Ans.clear();
        AnsLines.clear();
        for (int i = 0; i < 5; i++)
            data[i].clear();
    }
};

struct oneKalmanData {  // kalman data
    locationCoor x_t   = {0,0,0};
    locationCoor v_t   = {0,0,0};
    locationCoor x_t_1 = {0,0,0};
    locationCoor v_t_1 = {0,0,0};
    dType K            = 0.9f;
    dType Time;
    bool isInitialized = false;
};

struct storeTagInfo {
    int tagId{-1};
    bool isTagPosInitialed{false};
    QMap<QString, storeMethodInfo> methodInfo;

    // updated in calculation
    oneKalmanData calcPosKalmanData;

    QVector<dType>                 Reliability; //1.meas：长度之和；2.kalman：卡尔曼增益K
    QVector<dType>                 data_R;
    QVector<dType>                 data_P;
    QVector<dType>                 data_y;
    QVector<QVector<bool>>         usedSeneor;
    QVector<QVector<QPointF>>      iterPoints;
    QVector<QVector<dType>>        weight;

    QVector<QVector<locationCoor>> RawPoints;
    QVector<QVector<locationCoor>> RefinedPoints;

    CALC_POS_TYPE                  calcPosType;
    QString toString() {
        QString ans = QString("tagId:%0. ").arg(tagId, 3);
        foreach (storeMethodInfo i, methodInfo) {
            ans += QString("{%0|point Num: %1, line Num: %2|} ")
                    .arg(i.methodName)
                    .arg(i.Ans.count())
                    .arg(i.AnsLines.count());
        }
        return ans;
    }

    void clear() {
        iterPoints.clear();
        usedSeneor.clear();
        methodInfo.clear();
        RawPoints.clear();
        RefinedPoints.clear();
        weight.clear();
        calcPosType = CALC_POS_TYPE::POS_NONE;
    }
    void addOrResetMethodInfo(const QString &methodType, const QString &method);
    void reset(const QString &methodType, const QString &method);
    void reset(const QString &methodType);
};

class showStore
{
public:
    showStore();

    void addNewTagInfo(int tagId);
    storeTagInfo * getTagInfo(int tagId);

    void addRawPoints(int tagId, QVector<locationCoor> points);
    void clearRawPoints(int tagId);

    void addAnsPoint(int tagId, const QString &method, locationCoor p);
    void clearAnsPoints(int tagId, const QString &method);

    void clear() {
        foreach (storeTagInfo *info, tags) {
            info->clear();
            delete info;
        }
        tags.clear();
    }

    QMap<int, storeTagInfo *> tags;
};

#endif // SHOWSTORE_H
