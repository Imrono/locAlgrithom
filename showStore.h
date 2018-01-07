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

struct storeTagInfo {
    int tagId{-1};
    bool isTagActive{true};
    QMap<QString, storeMethodInfo> methodInfo;

    QVector<dType>                 Reliability; //1.meas：长度之和；2.kalman：卡尔曼增益K
    QVector<dType>                 data_R;
    QVector<dType>                 data_P;
    QVector<dType>                 data_y;

    QVector<QVector<locationCoor>> RawPoints;
    QVector<QVector<locationCoor>> RefinedPoints;

    CALC_POS_TYPE                  calcPosType;
    QString toString() {
        QString ans = QString("tagId:%0. ").arg(tagId, 3);
        foreach (storeMethodInfo i, methodInfo) {
            ans += QString("{%0|point Num: %1., line Num: %2|} ")
                    .arg(i.methodName)
                    .arg(i.Ans.count())
                    .arg(i.AnsLines.count());
        }
        return ans;
    }
    void reset(const QString &method) {
        if (methodInfo.contains(method)) {
            methodInfo[method].Ans.clear();
            methodInfo[method].AnsLines.clear();
        }
    }
    void clear() {
        methodInfo.clear();
        RawPoints.clear();
        RefinedPoints.clear();
        calcPosType = CALC_POS_TYPE::none_type;
    }
};

class showStore
{
public:
    showStore();

    void addNewTagInfo(int tagId);
    storeTagInfo * getTagInfo(int tagId);
    void addNewMethodInfo(int tagId, const QString &method);

    void addRawPoints(int tagId, QVector<locationCoor> points);
    void clearRawPoints(int tagId);

    void addAnsPoint(int tagId, const QString &method, locationCoor p);
    void clearAnsPoints(int tagId, const QString &method);

    QMap<int, storeTagInfo *> tags;
};

#endif // SHOWSTORE_H
