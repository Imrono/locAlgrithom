#include "dataInputLog.h"
#include <QFile>
#include <QDebug>

/*************************************************************/
dataInputLog::dataInputLog()
{
    q = new distanceData;
}

dataInputLog::~dataInputLog() {

}

void dataInputLog::loadNewFile(const int type, const QString &fileName) {
    if (1 == type) {
        loadNewFile_1(fileName);
    } else if (2 == type) {
        loadNewFile_2(fileName);
    }
}

void dataInputLog::loadNewFile_1(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }

    this->fileName = fileName;
    QString fileData = QString(file.readAll());
    QStringList strList = fileData.split("\n");

    // [2017/05/18 15:59:58:033] | 192.168.200.200| 00205-005-002426-00
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| (\\d{5})-(\\d{3})-(\\d{6})-(\\d{2})");

    analyzeDistanceData1(strList, rx);

    foreach (oneTag tag, q->tagsData) {
        if (tag.distData.count() > maxDataCount)
            maxDataCount = tag.distData.count();
    }

    //qDebug() << "#END ANALYZE# dataDistanceLog::loadNewFile_1 $> maxDataCount" << maxDataCount
    //         << "; tags Count" << q->tagsData.count();
}

void dataInputLog::loadNewFile_2(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    this->fileName = fileName;
    QString fileData = QString(file.readAll());
    QStringList strList = fileData.split("\n");

    // [2017/05/18 16:00:29:725] | 192.168.200.200| (000203):[001606,001290,001174,001323,002599,002284]
    // [002736-000312-000000]=>[002732-000317-000000] [03] [0-0]
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| \\((\\d{6})\\):"
                  "\\[(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6}),(\\d{6})\\] "
                  "\\[(\\d{6})-(\\d{6})-(\\d{6})\\]=>\\[(\\d{6})-(\\d{6})-(\\d{6})\\]");

    analyzeDistanceData2(strList, rx);

    foreach (oneTag tag, q->tagsData) {
        if (tag.distData.count() > maxDataCount)
            maxDataCount = tag.distData.count();
    }
}

void dataInputLog::analyzeDistanceData1(const QStringList &strList, const QRegExp &rx) {
    q->clear();
    oneLogData tmpLogData;
    int count = 0;
    int recCount = 0;
    foreach (QString str, strList) {
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
                recCount = 0;
                tmpLogData.distance.clear();
            }

            tmpLogData.time = QDateTime::fromString(dateStr, "yyyy/MM/dd hh:mm:ss:zzz");
            tmpLogData.distance.append(distStr.toInt());
            recCount ++;

            if (3 == sensorId && 4 == recCount) {
                //started = false;
                if (!q->tagsData.contains(tagId)) {
                    q->tagsData.insert(tagId, oneTag(tagId));
                }
                // TODO: refine the distance to 2D from 3D using Pythagoras
                q->tagsData[tagId].distData.append(tmpLogData);
                //qDebug() << "[@analyzeDistanceData1]" << count << tagId << q->tagsData.count() << tmpLogData.toString();
                count ++;
            }
        }
    }
    q->isInitialized = true;
}

void dataInputLog::analyzeDistanceData2(const QStringList &strList, const QRegExp &rx) {
    q->clear();
    foreach (QString str, strList) {
        //qDebug() << str;
        oneLogData tmpLogData;
        int pos = rx.indexIn(str);
        if (pos > -1 && 15 == rx.captureCount()) {
            tmpLogData.time = QDateTime::fromString(rx.cap(1), "yyyy/MM/dd hh:mm:ss:zzz");
            int tagId       = rx.cap(3).toInt();
            tmpLogData.distance.append(rx.cap(4).toInt());
            tmpLogData.distance.append(rx.cap(5).toInt());
            tmpLogData.distance.append(rx.cap(6).toInt());
            tmpLogData.distance.append(rx.cap(7).toInt());
            tmpLogData.distance.append(rx.cap(8).toInt());
            tmpLogData.distance.append(rx.cap(9).toInt());
            tmpLogData.p_t_1.x = rx.cap(10).toInt();
            tmpLogData.p_t_1.y = rx.cap(11).toInt();
            tmpLogData.p_t_1.z = rx.cap(12).toInt();
            tmpLogData.p_t.x = rx.cap(13).toInt();
            tmpLogData.p_t.y = rx.cap(14).toInt();
            tmpLogData.p_t.z = rx.cap(15).toInt();
            tmpLogData.l_t = QLineF(QPointF(tmpLogData.p_t_1.x, tmpLogData.p_t_1.y),
                                   QPointF(tmpLogData.p_t.x, tmpLogData.p_t.y));
            if (!q->tagsData.contains(tagId)) {
                oneTag tmpTag(tagId);
                q->tagsData.insert(tagId, tmpTag);
            }
            // TODO: refine the distance to 2D from 3D using Pythagoras
            q->tagsData[tagId].distData.append(tmpLogData);
            //qDebug() << [@analyzeDistanceData2]" << q->tagsData[tagId].tagId << tmpLogData.toString();
        }
    }
    q->isInitialized = true;
}

QString dataInputLog::toString() {
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

