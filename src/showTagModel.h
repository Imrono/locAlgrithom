#ifndef SHOWSTORE_H
#define SHOWSTORE_H
#include "_myheads.h"
#include <QVector>
#include <QMap>
#include "showTagDelegate.h"
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
    QVector<QDateTime>    time;
    QVector<locationCoor> Ans;
    QVector<QLineF>       AnsLines;

    enum DATA_STORED {
        STORED_MSE = 0,
        STORED_Crossed1 = 1,
        STORED_Crossed2 = 2,
        STORED_Kx  = 0,
        STORED_Rx  = 1,
        STORED_Px  = 2,
        NUM_METHOD_STORED = 3
    };
    // data[0]
    // meas: Reliability; kalman: R
    QVector<dType>        data[NUM_METHOD_STORED];

    void clear() {
        Ans.clear();
        AnsLines.clear();
        for (int i = 0; i < NUM_METHOD_STORED; i++)
            data[i].clear();
    }
};

struct oneKalmanData {  // kalman data
    locationCoor x_t   = {0,0,0};
    locationCoor v_t   = {0,0,0};
    locationCoor a_t   = {0,0,0};
    locationCoor x_t_1 = {0,0,0};
    locationCoor v_t_1 = {0,0,0};
    dType K            = 0.6f;  //Complementary Filter, K is the measure gain
    dType Time         = 0.f;
    bool isInitialized = false;
    void clear() {
        x_t   = {0,0,0};
        v_t   = {0,0,0};
        x_t_1 = {0,0,0};
        v_t_1 = {0,0,0};
        Time  = 0.f;
        isInitialized = false;
    }
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

    QVector<QPointF>               x_hat;
    bool                           isGaussPointAdded;

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
        isGaussPointAdded = false;
        x_hat.clear();
        calcPosKalmanData.clear();
    }
    void addOrResetMethodInfo(const QString &methodType, const QString &method);
    void reset(const QString &methodType, const QString &method);
    void reset(const QString &methodType);
};

class showTagModel
{
public:
    showTagModel();

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
