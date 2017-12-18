#include "mainwindow.h"
#include <QApplication>
#include "datatype.h"
#include <QFile>
#include <QVector>
#include <QDebug>
#include <calcPos.h>

int main(int argc, char *argv[])
{
    qDebug() << "test: {2,2,2}+{1,1,1} = " << (locationCoor(2,2,2)+locationCoor(1,1,1)).toString()
             << "{1,1,1}-{2,2,2} = " <<  (locationCoor(1,1,1)-locationCoor(2,2,2)).toString()
             << "{1,1,1} * 2 = " <<  (locationCoor(1,1,1)*2).toString()
             << "{1,1,1} / 2 = " <<  (locationCoor(1,1,1)/2).toString();
    QApplication a(argc, argv);

    //locationCoor loc[4] = {{0,0,0}, {300,0,0}, {300,200,0}, {0,200,0}};
    //185, 140
    locationCoor loc[4] = {{185+0,140+0,0}, {185+30,140+0,0}, {185+30,140+20,0}, {185+0,140+20,0}};
    for (int i = 0; i < 4; i++)
        qDebug() << QString("receiver location: %1 (%2,%3,%4)").arg(i)
                    .arg(loc[i].x, 6)
                    .arg(loc[i].y, 6)
                    .arg(loc[i].z, 6);
    calcPos calc(loc);
    MainWindow w(calc);
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
                    w.dist.append(tmpDist);
                }
                //qDebug() << tmpDist.distance[0] << tmpDist.distance[1] << tmpDist.distance[2] << tmpDist.distance[3];
            }
            //qDebug() << dateStr << ", " << tmpLoc << ", " << distStr << ", " << statStr << ", " << (count++)/4;
        }
    }
    qDebug() << "w.dist.count() =" << w.dist.count();

    w.calcPosVector();
    double measDist = w.calcTotalDistanceMeas();

    for(double Q = 0.0f; Q < 0.3; Q+= 0.002) {
        //w.calcKalmanPosVector(Q);
        //qDebug() << Q << "$$ measDist:" << measDist << "kalmanDist:" << w.calcTotalDistanceKalman();

    }

    w.calcKalmanPosVector(0.014f);
    double kalmanDist = w.calcTotalDistanceKalman();
    qDebug() << "w.calcTotalDistanceKalman() => measDist:" << measDist << "kalmanDist:" << kalmanDist;

    w.show();

    //calc.calcPosFromDistance(dist[0].distance);
    //w.calcFromDist(w.dist[0].distance);

    return a.exec();
}
