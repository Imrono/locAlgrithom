#ifndef ZFAKEDATA_H
#define ZFAKEDATA_H
#include "_myheads.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"
#include "dataType.h"

class zFakeData
{
public:
    zFakeData();

    void addOneSensor(locationCoor &sensor);
    void removeOneSensor(locationCoor &sensor);
    void addOneDistance(dType d, locationCoor &sensor);
    void removeOneDistance(locationCoor &sensor);

    QSettings   fakeCfgData;
    QStringList fakeDstData;
};

#endif // ZFAKEDATA_H
