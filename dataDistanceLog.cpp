#include "dataDistanceLog.h"
#include <QFile>
#include <QDebug>

dataDistanceLog::dataDistanceLog()
{
    q = new distanceData;
}

dataDistanceLog::~dataDistanceLog() {

}

void dataDistanceLog::loadNewFile(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }

    labelDistance tmpDist;
    int count = 0;
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);

        if (str.contains("| 00001-00")) {
            QString dateStr = str.mid(1, 23);
            QString numbStr = str.mid(53, 1);
            QString distStr = str.mid(55, 6);
            QString statStr = str.mid(62, 2);

            int tmpLoc = numbStr.toInt();
            tmpDist.distance[tmpLoc] = distStr.toInt();
            tmpDist.status[tmpLoc] = statStr.toInt();
            tmpDist.time[tmpLoc] = QDateTime::fromString(dateStr, "yyyy/MM/dd hh:mm:ss:zzz");

            if(3 == tmpLoc) {
                if (count < 1) {
                    count ++;
                    continue;
                } else {
                    count ++;
                    q->dist.append(tmpDist);
                }
            }
        }
    }
}

QString dataDistanceLog::toString() {
    return QString("fileName:%0, q->dist.count():%1").arg(fileName).arg(q->dist.count());
}
