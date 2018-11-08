#ifndef DATADISTANCECAN_IPC_H
#define DATADISTANCECAN_IPC_H
#include "dataInputBase.h"
#include "_myheads.h"
#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QDataStream>
#include <QBuffer>
#include <QTimer>

#define SHARE_MEMORY_NAME   "CAN_IPC_Cshapr_2_QT"
#define MAX_DISTANCE_DATA_LEN   3600

struct ST_SAV_PARSED_DATA {
    uint8_t batteryStatus;
    uint8_t bypassFlag;
    uint8_t isOperArea;
    uint8_t anchorIdx;
    uint16_t tagID;
    uint16_t distance;

    qint64 timeTick;
};

class dataInputCan : public dataInputBase
{
    Q_OBJECT
public:
    dataInputCan();
    ~dataInputCan();

    void AttatchToSharedMemory();

    bool getOneDistanceData(ST_SAV_PARSED_DATA &distData);

    void startDistanceFetchTimer();
    void stopDistanceFetchTimer();

private:
    QSharedMemory smem;
    QBuffer dataBuf;
    QBuffer indexBuf;
    int queueIndex{0};
    uint64_t m_ipcDataCnt{0};

    const int ONE_CAN_DISTANCE_DATA_LEN{16};

    QTimer *distanceFetchTimer;
    QVector<ST_SAV_PARSED_DATA> calcDistanceData;

private slots:
    void distanceDataFetch();
};

#endif // DATADISTANCECAN_IPC_H
