#include "_myheads.h"
#include "dataType.h"
#include "dataRecv.h"
#include "calcTagPos.h"
#include "calcTagTrack.h"

dataRecv::dataRecv()
{
    q = new distanceData;

    udpSocket.bind(QHostAddress::AnyIPv4, portNum);
    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(msgRecv()));

    // [2017/05/18 15:59:58:033] | 192.168.200.200| 00205-005-002426-00
    rx.setPattern("\\[(.*)\\] \\|\\s(.*)\\| (\\d{5})-(\\d{3})-(\\d{6})-(\\d{2})");
}

void dataRecv::msgRecv() {
    QByteArray array;
    QHostAddress address;
    quint16 port;
    array.resize(udpSocket.bytesAvailable());
    udpSocket.readDatagram(array.data(), array.size(), &address, &port);

    dataHandling(array);
}

void dataRecv::dataHandling(const QByteArray &dataIn) {
    int tagID;
    oneLogData tmpRecvData;
    q->tagsData[tagID].distData.append(tmpRecvData);

    int *dist;
    dType MSE;
    dType T;
    locationCoor lastPos;
    bool usedSensor[MAX_SENSOR]{true};
    oneKalmanData kalmanData;
    QVector<QPointF> iterTrace;
    QVector<dType> weight;
    QPointF x_hat;
    //locationCoor ans = calcOnePosition(dist, MSE, T, lastPos, kalmanData,
    //                                   usedSensor, iterTrace, weight, x_hat);


}
