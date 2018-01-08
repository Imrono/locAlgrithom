#include "dataDistanceLog.h"
#include <QFile>
#include <QDebug>

QString oneLogData::toString() {
    QString ans = QString("[%0]").arg(time.toString("yyyy/MM/dd hh:mm:ss:zzz"));
    for (int i = 0; i < distance.count(); i++) {
        ans += QString("{%0,%1}").arg(i).arg(distance[i], 6, 10, QChar('0'));
    }
    return ans;
}

/*************************************************************/
dataDistanceLog::dataDistanceLog()
{
    q = new distanceData;
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
    //qDebug() << "#BEGIN ANALYZE# dataDistanceLog::loadNewFile_1 $> fileName" << fileName;

    // [2017/05/18 15:59:58:033] | 192.168.200.200| 00205-005-002426-00
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| (\\d{5})-(\\d{3})-(\\d{6})-(\\d{2})");

    oneLogData tmpLogData;
    int count = 0;
    bool started = false;
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);

        //qDebug() << str;
        int pos = rx.indexIn(str);
        if (pos > -1 && 6 == rx.captureCount()) {
            QString dateStr = rx.cap(1);
            int tagId    = rx.cap(3).toInt();
            int sensorId = rx.cap(4).toInt();
            QString distStr = rx.cap(5);
            QString statStr = rx.cap(6);
            Q_UNUSED(statStr);

            if (0 == sensorId) {
                started = true;
                tmpLogData.distance.clear();
            }

            tmpLogData.time = QDateTime::fromString(dateStr, "yyyy/MM/dd hh:mm:ss:zzz");
            tmpLogData.distance.append(distStr.toInt());

            if (3 == sensorId && started) {
                //started = false;
                if (!q->tagsData.contains(tagId)) {
                    q->tagsData.insert(tagId, oneTag(tagId));
                }
                q->tagsData[tagId].distData.append(tmpLogData);
                //qDebug() << "dataDistanceLog::loadNewFile_1 $>" << count << tagId << q->tagsData.count() << tmpLogData.toString();
                count ++;
            }
        }
    }
    q->isInitialized = true;

    foreach (oneTag tag, q->tagsData) {
        if (tag.distData.count() > maxDataCount)
            maxDataCount = tag.distData.count();
    }

    //qDebug() << "#END ANALYZE# dataDistanceLog::loadNewFile_1 $> maxDataCount" << maxDataCount
    //         << "; tags Count" << q->tagsData.count();
    composeMeasData();
}

void dataDistanceLog::loadNewFile_2(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    this->fileName = fileName;
    // [2017/05/18 16:00:29:725] | 192.168.200.200| (000203):[001606,001290,001174,001323,002599,002284]
    // [002736-000312-000000]=>[002732-000317-000000] [03] [0-0]
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| \\((\\d{6})\\):\\[(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6})\\]");

    int tagId = -1;
    while(!file.atEnd()) {
        oneLogData tmpLogData;

        QByteArray line = file.readLine();
        QString str(line);
        //qDebug() << str;

        int pos = rx.indexIn(str);
        if (pos > -1 && 9 == rx.captureCount()) {
            tmpLogData.time = QDateTime::fromString(rx.cap(1), "yyyy/MM/dd hh:mm:ss:zzz");
            tagId           = rx.cap(3).toInt();
            tmpLogData.distance.append(rx.cap(4).toInt());
            tmpLogData.distance.append(rx.cap(5).toInt());
            tmpLogData.distance.append(rx.cap(6).toInt());
            tmpLogData.distance.append(rx.cap(7).toInt());
            tmpLogData.distance.append(rx.cap(8).toInt());
            tmpLogData.distance.append(rx.cap(9).toInt());
            if (!q->tagsData.contains(tagId)) {
                oneTag tmpTag(tagId);
                q->tagsData.insert(tagId, tmpTag);
            }
            q->tagsData[tagId].distData.append(tmpLogData);
            //qDebug() << q->tagsData[tagId].tagId << tmpLogData.toString();
        }
    }
    q->isInitialized = true;
}

void dataDistanceLog::composeMeasData() {

}

QString dataDistanceLog::toString() {
    QString ans = QString("dataDistanceLog::toString $> fileName:%0, tags:%1 | ")
            .arg(fileName)
            .arg(q->tagsData.count());

    QMapIterator<int, oneTag> iter(q->tagsData);
    while (iter.hasNext()) {
        iter.next();
        ans += QString("{tagId:%0,count:%1}").arg(iter.key()).arg(iter.value().distData.count());
    }
    return ans;
}
