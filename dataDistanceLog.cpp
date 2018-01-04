#include "dataDistanceLog.h"
#include <QFile>
#include <QDebug>

dataDistanceLog::dataDistanceLog()
{
    q = new distanceData;

    // [2017/05/18 16:00:29:725] | 192.168.200.200| (000203):[001606,001290,001174,001323,002599,002284]
    // [002736-000312-000000]=>[002732-000317-000000] [03] [0-0]
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| \\((\\d{6})\\):\\[(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6})\\]");
}

dataDistanceLog::~dataDistanceLog() {

}

void dataDistanceLog::loadNewFile_1(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    this->fileName = fileName;

    oneLogData_1 tmpLogData;
    labelDistance tmpDist;
    int count = 0;
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);

        //qDebug() << str;
        int pos = rx.indexIn(str);
        if (pos > -1 && 6 == rx.captureCount()) {
            QString dateStr = rx.cap(1);
            QString idexStr = rx.cap(3);
            QString sensStr = rx.cap(4);
            QString distStr = rx.cap(5);
            QString statStr = rx.cap(6);

            int tmpLoc = sensStr.toInt();
            tmpDist.distance[tmpLoc] = distStr.toInt();
            tmpDist.status[tmpLoc] = statStr.toInt();
            tmpDist.time[tmpLoc] = QDateTime::fromString(dateStr, "yyyy/MM/dd hh:mm:ss:zzz");

            int idxVector = q->findTagByIdx_1(idexStr.toInt());
            tmpLogData.time      = QDateTime::fromString(dateStr, "yyyy/MM/dd hh:mm:ss:zzz");
            tmpLogData.distance  = distStr.toInt();
            tmpLogData.sensorIdx = sensStr.toInt();
            tmpLogData.status    = statStr.toInt();
            q->tagsData_1[idxVector].distData.append(tmpLogData);
            //qDebug() << tmpLogData.toString();

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
    q->isInitialized = true;

    composeMeasData();
}

void dataDistanceLog::loadNewFile_2(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    this->fileName = fileName;

    while(!file.atEnd()) {
        oneLogData_2 tmpLogData;

        QByteArray line = file.readLine();
        QString str(line);

        //qDebug() << str;
        int pos = rx.indexIn(str);
        if (pos > -1 && 9 == rx.captureCount()) {
            int idxVector = q->findTagByIdx_2(rx.cap(3).toInt());
            tmpLogData.time = QDateTime::fromString(rx.cap(1), "yyyy/MM/dd hh:mm:ss:zzz");
            tmpLogData.distance.append(rx.cap(4).toInt());
            tmpLogData.distance.append(rx.cap(5).toInt());
            tmpLogData.distance.append(rx.cap(6).toInt());
            tmpLogData.distance.append(rx.cap(7).toInt());
            tmpLogData.distance.append(rx.cap(8).toInt());
            tmpLogData.distance.append(rx.cap(9).toInt());
            q->tagsData_2[idxVector].distData.append(tmpLogData);
            qDebug() << q->tagsData_2[idxVector].tagIdx << tmpLogData.toString();
        }
    }
    q->isInitialized = true;
}

void dataDistanceLog::composeMeasData() {

}

QString dataDistanceLog::toString() {
    QString ans = QString("dataDistanceLog $> fileName:%0, q->dist.count():%1, tags:%2 $> ")
            .arg(fileName).arg(q->dist.count())
            .arg(q->tagsData_1.count());
    for (int i = 0; i < q->tagsData_1.count(); i++) {
        ans += QString("idx:%0, count:%1; ").arg(q->tagsData_1[i].tagIdx).arg(q->tagsData_1[i].distData.count());
    }
    return ans;
}
