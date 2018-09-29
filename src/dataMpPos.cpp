#include <QDebug>
#include "dataMpPos.h"
#include "showTagModel.h"
#include "uiUsrFrame.h"
#include "uiMainWindow.h"
#include "ui_mainwindow.h"

dataMpPos::dataMpPos()
{
    udpSocket.bind(selfAddr, portNum);
    //udpSocket.bind(QHostAddress("127.0.0.1"), portNum);
    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(msgRecv()));

    sendTimer = new QTimer(this);
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(msgSend()));
}

void dataMpPos::startMpReqTimer() {
    if (!sendTimer->isActive()) {
        sendTimer->setSingleShot(false);
        sendTimer->start(300);
        qDebug() << "[startMpReqTimer]";
    }
}

void dataMpPos::stopMpReqTimer() {
    if (sendTimer->isActive()) {
        sendTimer->stop();
        qDebug() << "[stopMpReqTimer]";
    }
}

void dataMpPos::msgSend() {
    static unsigned short seqNum  = 0;
    QByteArray inspByteArr = getPsQueryCmdArray(0/*seqNum ++*/);
    udpSocket.writeDatagram(inspByteArr, mpAddr, portNum);

    qDebug() << "[msgSend]" << seqNum << mpAddr << portNum;
/*
    qDebug() << QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10")
                .arg(static_cast<int>(inspByteArr[0]))
                .arg(static_cast<int>(inspByteArr[1]))
                .arg(static_cast<int>(inspByteArr[2]))
                .arg(static_cast<int>(inspByteArr[3]))
                .arg(static_cast<int>(inspByteArr[4]))
                .arg(static_cast<int>(inspByteArr[5]))
                .arg(static_cast<int>(inspByteArr[6]))
                .arg(static_cast<int>(inspByteArr[7]))
                .arg(static_cast<int>(inspByteArr[8]))
                .arg(static_cast<int>(inspByteArr[9]));
*/
}

void dataMpPos::msgRecv() {
    QByteArray array;
    QHostAddress address;
    quint16 port;
    array.resize(udpSocket.bytesAvailable());
    udpSocket.readDatagram(array.data(), array.size(), &address, &port);

    //qDebug() << array;

    int rst = checkResponseData(array);
    if (1 == rst) {
        dataHandling(array);
    } else {
        qDebug() << "checkResponseData failed" << rst;
    }
}

int dataMpPos::checkResponseData(const QByteArray &dataIn)  //检验返回数据是否有效
{
    if (dataIn[0] != 'J' || dataIn[1] != 'G') {
        return -1;  //如果返回的报文标示不是"JG"，那么返回-1
    }
    if ((((short)dataIn[2] << 8) + dataIn[3]) != dataIn.length() - 6) {
        return -2;  //如果第三第四两位组成的0xXXXX不是总长度减6（开头四位，加上校验的两位），返回-2
    }
    if (dataIn[4] != 'l') {
        return -3;  //如果返回的响应码不是"l"，返回-3
    }
    if (dataIn[7] != 0x00 && dataIn[7] != 0x01) {
        return -4;  //如果命令响应不是0或者1，返回-4
    }
    return 1;
}

void dataMpPos::dataHandling(const QByteArray &dataIn) {
    showTagModel &store = parent->getStore();
    // tag info
    int tagNum = dataIn[8];
    qDebug() << "nTag" << tagNum;

    for (int i = 0; i < tagNum; i++) {
        QByteArray oneTagMsg;
        oneTagMsg.append(dataIn.data() + 9 + i * 12, 12);
        //qDebug() << oneTagMsg;

        uint8_t operArea    = ((uint8_t)oneTagMsg[0] >> 7) & 0x01;
        uint8_t areaInfo    = ((uint8_t)oneTagMsg[0] >> 5) & 0x03;
        uint8_t lackBattery = ((uint8_t)oneTagMsg[0] >> 0) & 0x01;

        uint16_t tagId      = (uint16_t)oneTagMsg[3] * 256 + (uint8_t)oneTagMsg[2];

        int x = (uint16_t)oneTagMsg[4] * 256 + (uint8_t)oneTagMsg[5];
        int y = (uint16_t)oneTagMsg[6] * 256 + (uint8_t)oneTagMsg[7];
        int z = (uint16_t)oneTagMsg[8] * 256 + (uint8_t)oneTagMsg[9];

        if (!store.tags.contains(tagId)) {
            store.addNewTagInfo(tagId);
            store.tags[tagId]->addOrResetMethodInfo(MP_POS_STR, CALC_POS2STR[store.calcPosType]);
            store.tags[tagId]->calcPosType = CALC_POS_TYPE::Mp_Pos_In;
        }

        storeMethodInfo &measInfo = store.tags[tagId]->methodInfo[MP_POS_STR];
        measInfo.Ans.append(locationCoor(x, y, z));
        int nCnt = measInfo.Ans.count();
        if (nCnt > 1) {
            QLineF l_p = QLineF(measInfo.Ans[nCnt-2].toQPointF(), measInfo.Ans[nCnt-1].toQPointF());
            measInfo.AnsLines.append(l_p);
        }
        store.tags[tagId]->operInfo    = operArea;
        store.tags[tagId]->areaInfo    = areaInfo;
        store.tags[tagId]->batteryInfo = lackBattery;

        uiUsrFrame *usrFrame = &(parent->getUsrFrame());
        if (!usrFrame->containTagId(tagId)) {
            usrFrame->addOneUsr(tagId);
            parent->ui->canvas->syncWithUiFrame(usrFrame);
        }

        parent->ui->canvas->setPosition(tagId, MP_POS_STR, QPointF(x, y));
        qDebug() << "tagId:" << tagId << "pos:" << x << y;
    }

    //获取主控器状态：
    QByteArray mpStateMsg;
    mpStateMsg.append(27 + 12 * tagNum, 6);
    uint8_t mpAlarmState = (mpStateMsg[5] >> 6) & 0x03;

    qDebug() << "mpAlarmState" << mpAlarmState;

    if (dataIn[7] == 0x01) {    // isSendSecondReq
        msgSend();
    } else
    if (dataIn[7] == 0x00) {    // trigger presentation
        qDebug() << "update";
        parent->ui->canvas->update();
        //parent->update();
    } else {}
}

QByteArray dataMpPos::getPsQueryCmdArray(int cmdnum) {  //以自增的命令序号（最大为65535）作为参数，返回一条完整的查询命令
    QByteArray queryCmd(10, 0);
    queryCmd[0] = 0x4a;
    queryCmd[1] = 0x47; //先装填上“JG”
    queryCmd[2] = 0x00;
    queryCmd[3] = 0x04; //命令长度为4
    queryCmd[4] = 0x4c; //查询的命令码“L”
    queryCmd[5] = (char)(cmdnum % 256);
    queryCmd[6] = (char)((cmdnum >> 8) & 0xff); //按照协议，低字节在前，高字节在后表示命令序号
    queryCmd[7] = 0x00;
    int check0 = 0x00, check1 = 0x00;
    for (int i = 0; i < 8; i++) {   //由前八位获得校验码
        if (0 == i % 2) {
            check0 += queryCmd[i] & 0xff;
        } else {
            check1 += queryCmd[i] & 0xff;
        }
    }
    queryCmd[8] = (char)(check0 & 0xff);
    queryCmd[9] = (char)(check1 & 0xff);
    return queryCmd;
}
