#include "dataSensorIni.h"
#include <QFile>

QString configData::toString() {
    QString str;
    // [Sensor]
    str += QString("Sensor: Cnt=%0\r\n").arg(sensor.count());
    for (int i = 0; i < sensor.count(); i++) {
        str += QString("coor{%0}:%1 ").arg(i).arg(sensor[i].toString());
    }
    str += QString("\r\n");
    // [Stop]
    str += QString("[Stop] AreaCnt=%0\r\n").arg(stop.count());
    for (int i = 0; i < stop.count(); i++) {
        str += QString("[Stop%0]: Cnt=%1\r\n").arg(i).arg(stop[i].count());
        for (int j = 0; j < stop[i].count(); j++) {
            str += QString("coor{%0}:%1 ").arg(j).arg(stop[i][j].toString());
        }
        str += QString("\r\n");
    }
    // [Alarm]
    str += QString("[Alarm] AreaCnt=%0\r\n").arg(alarm.count());
    for (int i = 0; i < alarm.count(); i++) {
        str += QString("[Alarm%0]: Cnt=%1\r\n").arg(i).arg(alarm[i].count());
        for (int j = 0; j < alarm[i].count(); j++) {
            str += QString("coor{%0}:%1 ").arg(j).arg(alarm[i][j].toString());
        }
        str += QString("\r\n");
    }
    // [Oper]
    str += QString("[Oper] AreaCnt=%0\r\n").arg(oper.count());
    for (int i = 0; i < oper.count(); i++) {
        str += QString("[Oper%0]: Cnt=%1\r\n").arg(i).arg(oper[i].count());
        for (int j = 0; j < oper[i].count(); j++) {
            str += QString("coor{%0}:%1 ").arg(j).arg(oper[i][j].toString());
        }
        str += QString("\r\n");
    }

    return str;
}

dataSensorIni::dataSensorIni()
{
    q = new configData;
}
dataSensorIni::~dataSensorIni() {
    if (nullptr == q)
        delete q;
}

void dataSensorIni::loadNewFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.exists()) {
        qDebug() << "dataSensorIni file not exist";
        return;
    }

    QSettings iniSetting{fileName, QSettings::IniFormat};
    this->fileName = fileName;
    q->reset();

    int Cnt = 0;
    int AreaCnt = 0;

    // [Sensor]
    iniSetting.beginGroup("/Sensor");
    Cnt = iniSetting.value("/Cnt").toInt();
    for(int coorIdx = 0; coorIdx < Cnt; coorIdx++) {
        q->sensor.append({iniSetting.value(my2Str("x", coorIdx)).toFloat(),
                          iniSetting.value(my2Str("y", coorIdx)).toFloat(),
                          iniSetting.value(my2Str("z", coorIdx)).toFloat()});
    }
    iniSetting.endGroup();

    // [Stop]
    AreaCnt = iniSetting.value("/Stop/AreaCnt").toInt();
    for(int i = 0; i < AreaCnt; i++) {
        iniSetting.beginGroup(my2Str("/Stop", i));
        Cnt = iniSetting.value("/Cnt").toInt();
        QVector<locationCoor> tmp;
        for (int coorIdx = 0; coorIdx < Cnt; coorIdx++) {
            tmp.append({iniSetting.value(my2Str("x", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("y", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("z", coorIdx)).toFloat()});
        }
        q->stop.append(tmp);
        iniSetting.endGroup();
    }

    // [Alarm]
    AreaCnt = iniSetting.value("/Alarm/AreaCnt").toInt();
    for(int i = 0; i < AreaCnt; i++) {
        iniSetting.beginGroup(my2Str("/Alarm", i));
        Cnt = iniSetting.value("/Cnt").toInt();
        QVector<locationCoor> tmp;
        for (int coorIdx = 0; coorIdx < Cnt; coorIdx++) {
            tmp.append({iniSetting.value(my2Str("x", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("y", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("z", coorIdx)).toFloat()});
        }
        q->alarm.append(tmp);
        iniSetting.endGroup();
    }

    // [Oper]
    AreaCnt = iniSetting.value("/Oper/AreaCnt").toInt();
    for(int i = 0; i < AreaCnt; i++) {
        iniSetting.beginGroup(my2Str("/Oper", i));
        Cnt = iniSetting.value("/Cnt").toInt();
        QVector<locationCoor> tmp;
        for (int coorIdx = 0; coorIdx < Cnt; coorIdx++) {
            tmp.append({iniSetting.value(my2Str("x", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("y", coorIdx)).toFloat(),
                        iniSetting.value(my2Str("z", coorIdx)).toFloat()});
        }
        q->oper.append(tmp);
        iniSetting.endGroup();
    }
    qDebug() << toString();
}

QString dataSensorIni::toString() {
    QString str;
    str += QString("fileName:%0\r\n").arg(fileName);
    str += q->toString();
    return str;
}
