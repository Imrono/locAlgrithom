#include "mainwindow.h"
#include <QApplication>
#include "datatype.h"
#include <QFile>
#include <QVector>
#include <QDebug>
#include <calcPos.h>
#include "showStore.h"
#include "kalmanCalc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    locationCoor loc[4] = {{185+0,140+0,0}, {185+30,140+0,0}, {185+30,140+20,0}, {185+0,140+20,0}};
    for (int i = 0; i < 4; i++)
        qDebug() << QString("receiver location: %1 (%2,%3,%4)").arg(i)
                    .arg(loc[i].x, 6)
                    .arg(loc[i].y, 6)
                    .arg(loc[i].z, 6);
    calcPos calc(loc);

    showStore store(loc);
    store.appendLabel(MEASUR_STR,
                      showPoint(4, QPen(QColor(0, 160, 230), 2), QBrush(QColor(255, 160, 90)),
                                   QPen(Qt::black, 1), QBrush(QColor(255, 160, 90))));
    store.appendLabel(KALMAN_STR, showPoint(4, QPen(Qt::gray, 2), QBrush(Qt::darkGreen)));

    MainWindow w(&store);
    labelDistance tmpDist;
    //QFile file("D:\\code\\kelmanLocationData\\201712111501.log");
    QFile file("D:\\code\\kelmanLocationData\\201712111515.log");
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
    qDebug() << "calcPos.dist.count() =" << calc.dist.count();

    calc.calcPosVector(store.getLabel(MEASUR_STR));
    calc.calcPotimizedPos(store.getLabel(MEASUR_STR));
    double measDist = calcTotalAvgDistanceSquare(store.getLabel(MEASUR_STR)->AnsLines);
    qDebug() << store.getLabel(MEASUR_STR)->toString();

    kalmanCalc::calcKalmanPosVector(store.getLabel(MEASUR_STR), store.getLabel(KALMAN_STR), 0.014f);
    double kalmanDist = calcTotalAvgDistanceSquare(store.getLabel(KALMAN_STR)->AnsLines);
    qDebug() << store.getLabel(KALMAN_STR)->toString();
    qDebug() << "w.calcTotalDistanceKalman() => measDist:" << measDist << "kalmanDist:" << kalmanDist;

    w.show();

    //calc.calcPosFromDistance(dist[0].distance);
    //w.calcFromDist(w.dist[0].distance);

    return a.exec();
}
