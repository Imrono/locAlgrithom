#include "dataInputCan.h"
#include <QDateTime>
#include "uiUsrFrame.h"
#include "uiMainWindow.h"
#include "ui_mainwindow.h"

dataInputCan::dataInputCan() {
    timebase = QDateTime::currentDateTime();

    smem.setNativeKey(SHARE_MEMORY_NAME);
    if (smem.isAttached()) {
        qDebug() << "attached, detach when construction";
        smem.detach();
    }

    distanceFetchTimer = new QTimer(this);
    connect(distanceFetchTimer, SIGNAL(timeout()), this, SLOT(distanceDataFetch()));

    displayUpdateTimer = new QTimer(this);
    connect(displayUpdateTimer, SIGNAL(timeout()), this, SLOT(displayUpdate()));
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
    }
    if (!displayUpdateTimer->isActive()) {
        displayUpdateTimer->setSingleShot(false);
        displayUpdateTimer->start(DISPLAY_INTERVAL_TIME);
    }

    if (smem.isAttached()) {
        qDebug() << "attached, detach when attatch";
        smem.detach();
    }
    smem.attach(QSharedMemory::ReadOnly);
    qDebug() << "[dataInputCan] start -> attatch";
}

void dataInputCan::stopDistanceFetchTimer() {
    if (distanceFetchTimer->isActive()) {
        distanceFetchTimer->stop();
    }
    if (displayUpdateTimer->isActive()) {
        displayUpdateTimer->stop();
    }

    smem.detach();
    qDebug() << "[dataInputCan] stop -> detach";
}

void dataInputCan::displayUpdate() {
    mainWin->handleModelDataUpdate(false, true);
}

bool dataInputCan::getOneDistanceData(ST_SAV_PARSED_DATA &distData) {
    indexBuf.setData((const char*)(smem.constData()), 8);
    uint64_t dataLength = 0;
    memcpy(&dataLength, indexBuf.data().data(), 8);

    if (dataLength == m_ipcDataCnt) {
        return false;
    } else {
        //qDebug() << "[@dataInputCan::getOneDistanceData]" << dataLength << m_ipcDataCnt;
        if (0 == m_ipcDataCnt) {
            m_ipcDataCnt = dataLength;
            queueIndex = dataLength % MAX_DISTANCE_DATA_LEN;
            //qDebug() << "[@dataInputCan::getOneDistanceData]" << "1st" << dataLength << queueIndex;
        } else {
            m_ipcDataCnt ++;
        }
        dataBuf.setData((const char*)(smem.constData()) + ONE_CAN_DISTANCE_DATA_LEN * queueIndex + 8, ONE_CAN_DISTANCE_DATA_LEN);
        queueIndex = (queueIndex + 1) % MAX_DISTANCE_DATA_LEN;

        memcpy(&distData, dataBuf.data().data(), 8);
        //qDebug() << "aa" << distData.tagID << distData.anchorIdx << distData.timeTick;

        return true;
    }
}

void dataInputCan::distanceDataFetch() {
    //qDebug() << QDateTime::currentMSecsSinceEpoch();
    ST_SAV_PARSED_DATA oneDistData;
    while (getOneDistanceData(oneDistData)) {
        int isTriggerPosCalc = 0;
        if (calcDistanceData.count() > 0) {
            // other tagID comes
            if (calcDistanceData[0].tagID != oneDistData.tagID) {
                isTriggerPosCalc = 2;
            } else
            // 50ms threshold, can update is 10ms, ipc update is 20 ms, resolution 0.1ms
            if (oneDistData.timeTick - calcDistanceData[0].timeTick > 500) {
                isTriggerPosCalc = 3;
            } else
            // distance data is full
            if (calcDistanceData.count() == sensorNum - 1) {
                isTriggerPosCalc = 1;
                calcDistanceData.append(oneDistData);
            }
        }

        if (isTriggerPosCalc != 0){
            if (0 == timeOffset) {
                timeOffset = timebase.currentMSecsSinceEpoch() - calcDistanceData.last().timeTick / 10;
            }

            showTagModel &store = mainWin->getStore();
            uiUsrFrame &usrFrame = mainWin->getUsrFrame();

            int tagId = calcDistanceData[0].tagID;
            int distCount = calcDistanceData.count();
/********** TRIGGER CALC POS **************************************************/
            // 1. data prepare
            distData = oneLogData();
            for (int i{0}; i < sensorNum; i++) {
                bool isFound = false;
                for (int j{0}; j < distCount; j++) {
                    if (i == calcDistanceData[j].anchorIdx) {
                        distData.distance.append(static_cast<int>(calcDistanceData[j].distance));
                        isFound = true;
                        break;
                    }
                }
                if (!isFound) {
                    distData.distance.append(30000);    // max value for missing data
                }
            }

            distData.time = QDateTime::fromMSecsSinceEpoch(timeOffset + calcDistanceData.last().timeTick / 10);
            distData.p_t_1 = {0.0, 0.0, 0.0};
            bool isStoreTagUsed{false};
            storeTagInfo *tagInfo{nullptr};
            if (store.tags.contains(tagId)) {
                tagInfo = store.tags[tagId];
                if (tagInfo) {
                    isStoreTagUsed = true;
                    storeMethodInfo &canInfo = tagInfo->methodInfo[MEASUR_STR];
                    if (canInfo.Ans.count() > 0) {
                        distData.p_t_1 = canInfo.Ans.last();
                    }
                    tagInfo->calcPosType = store.calcPosType;
                }
            }
            if (!isStoreTagUsed) {
                store.tags.insert(tagId, new storeTagInfo());
                tagInfo = store.tags[tagId];
                tagInfo->clear();
                tagInfo->addOrResetMethodInfo(MEASUR_STR, CALC_POS2STR[store.calcPosType]);
                tagInfo->calcPosType = store.calcPosType;
                tagInfo->tagId = tagId;

                usrFrame.setBtnEnableLM(tagId, false);
                usrFrame.addOneUsr(tagId, USR_STATUS::HAS_DISTANCE_DATA);
                mainWin->ui->canvas->syncWithUiFrame(&usrFrame);
            }

            // 2. calculation
            qDebug() << "trigger" << distData.time.toString("yyyy/MM/dd hh:mm:ss:zzz") << isTriggerPosCalc << distCount << distData.distance;
            mainWin->posCalcPROCESS(store.calcPosType, tagId);

            // clear the vector and append first
            calcDistanceData.clear();
            if (2 == isTriggerPosCalc || 3 == isTriggerPosCalc) {
                calcDistanceData.append(oneDistData);
            }
        } else {
            calcDistanceData.append(oneDistData);
            //qDebug() << "[@dataInputCan::distanceDataFetch]" << calcDistanceData.count();
        }
    }
}
