#include "dataInputCan.h"
#include <QDateTime>
#include "uiUsrFrame.h"
#include "uiMainWindow.h"
#include "ui_mainwindow.h"

dataInputCan::dataInputCan() {
    smem.setNativeKey(SHARE_MEMORY_NAME);
    if (smem.isAttached()) {
        qDebug() << "attached, detach when construction";
        smem.detach();
    }

    distanceFetchTimer = new QTimer(this);
    connect(distanceFetchTimer, SIGNAL(timeout()), this, SLOT(distanceDataFetch()));
}

dataInputCan::~dataInputCan() {
    if (smem.isAttached()) {
        qDebug() << "attached, detach when deconstruction";
        smem.detach();
    }
}

void dataInputCan::startDistanceFetchTimer() {
    if (!distanceFetchTimer->isActive()) {
        distanceFetchTimer->setSingleShot(false);
        distanceFetchTimer->start(20);
        qDebug() << "[startDistFetchTimer]";
    }
}

void dataInputCan::stopDistanceFetchTimer() {
    if (distanceFetchTimer->isActive()) {
        distanceFetchTimer->stop();
        qDebug() << "[stopDistFetchTimer]";
    }
}

void dataInputCan::AttatchToSharedMemory() {
    if (smem.isAttached()) {
        qDebug() << "attached, detach when attatch";
        smem.detach();
    }
    smem.attach(QSharedMemory::ReadOnly);
}

bool dataInputCan::getOneDistanceData(ST_SAV_PARSED_DATA &distData) {
    indexBuf.setData((const char*)(smem.constData()), 4);
    uint64_t dataLength = 0;
    memcpy(&dataLength, indexBuf.data().data(), 8);

    if (dataLength == m_ipcDataCnt) {
        return false;
    } else {
        dataBuf.setData((const char*)(smem.constData()) + ONE_CAN_DISTANCE_DATA_LEN * queueIndex + 8, ONE_CAN_DISTANCE_DATA_LEN);
        queueIndex = (queueIndex + 1) % MAX_DISTANCE_DATA_LEN;
        m_ipcDataCnt ++;

        memcpy(&distData, dataBuf.data().data(), 8);
        distData.timeTick = QDateTime::currentMSecsSinceEpoch();

        return true;
    }
}

void dataInputCan::distanceDataFetch() {
    qDebug() << QDateTime::currentMSecsSinceEpoch();
    ST_SAV_PARSED_DATA distData;
    if (getOneDistanceData(distData)) {
        if (calcDistanceData.count() == mainWin->cfgData.get_q()->sensor.count() //full
         || calcDistanceData[0].tagID != distData.tagID) {                      // other tagID comes
            // triggle the position calculate

            // clear the vector
            calcDistanceData.clear();

            if (calcDistanceData[0].tagID != distData.tagID) {
                calcDistanceData.append(distData);
            }
        } else {
            calcDistanceData.append(distData);
        }
    } else {}
}
