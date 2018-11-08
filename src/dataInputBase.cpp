#include "dataInputBase.h"

QString oneLogData::toString() {
    QString ans = QString("[%0]").arg(time.toString("yyyy/MM/dd hh:mm:ss:zzz"));
    for (int i = 0; i < distance.count(); i++) {
        ans += QString("{%0,%1}").arg(i).arg(distance[i], 6, 10, QChar('0'));
    }
    ans += (p_t_1.toString() + p_t.toString());
    return ans;
}

dataInputBase::dataInputBase()
{

}
