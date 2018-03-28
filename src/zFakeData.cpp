#include "zFakeData.h"

zFakeData::zFakeData()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    fakeCfgData.setValue("sensor/Cnt", 0);
    fakeCfgData.setValue("Stop/AreaCnt", 0);
    fakeCfgData.setValue("Alarm/AreaCnt", 0);
    fakeCfgData.setValue("Oper/AreaCnt", 0);
}
