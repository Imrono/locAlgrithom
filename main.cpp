#include "uiMainWindow.h"
#include <QApplication>
#include <QSettings>
#include "datatype.h"
#include <QFile>
#include <QVector>
#include <QDebug>
#include <calcPos.h>
#include "showStore.h"
#include "calcKalman.h"
#include "dataSensorIni.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    dataSensorIni cfgData("D:\\code\\kelmanLocationData\\configExample.ini");
    qDebug() << cfgData.toString();
    calcPos calc(cfgData.get_q());

    showStore store;
    store.appendLabel(MEASUR_STR,
                      showTagRelated(4, QPen(QColor(0, 160, 230), 2), QBrush(QColor(255, 160, 90)),
                                   QPen(Qt::black, 1), QBrush(QColor(255, 160, 90))));
    store.appendLabel(KALMAN_STR, showTagRelated(4, QPen(Qt::gray, 2), QBrush(Qt::darkGreen)));

    uiMainWindow w(&store);
    w.setConfigData(cfgData.get_q());
    labelDistance tmpDist;
    QString fileName("D:\\code\\kelmanLocationData\\201712111515.log");
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
    }
    int count = 0;
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);

        if (str.contains("| 00001-00")) {
            //qDebug()<< str;
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
                    calc.dist.append(tmpDist);
                }
                //qDebug() << tmpDist.distance[0] << tmpDist.distance[1] << tmpDist.distance[2] << tmpDist.distance[3];
            }
            //qDebug() << dateStr << ", " << tmpLoc << ", " << distStr << ", " << statStr << ", " << (count++)/4;
        }
    }
    qDebug() << "fileName:" << fileName << ",calcPos.dist.count():" << calc.dist.count();

    calc.calcPosVector(store.getLabel(MEASUR_STR));
    //calc.calcPosVectorWylie(store.getLabel(MEASUR_STR));
    calc.calcPotimizedPos(store.getLabel(MEASUR_STR));
    dType measDist = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    qDebug() << store.getLabel(MEASUR_STR)->toString();

    //calcKalman::calcKalmanPosVectorModified(store.getLabel(MEASUR_STR), store.getLabel(KALMAN_STR));
    calcKalman::calcKalmanPosVector(store.getLabel(MEASUR_STR), store.getLabel(KALMAN_STR));
    dType kalmanDist = calcTotalAvgDistanceSquare(store.getLabel(KALMAN_STR)->AnsLines);
    qDebug() << store.getLabel(KALMAN_STR)->toString();
    qDebug() << "avgDistanceSquare => measDist:" << measDist << "; kalmanDist:" << kalmanDist;

    w.show();

    //calc.calcPosFromDistance(dist[0].distance);
    //w.calcFromDist(w.dist[0].distance);

    return a.exec();
}
