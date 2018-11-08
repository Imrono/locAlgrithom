#ifndef DATAMPPOS_H
#define DATAMPPOS_H
#include "dataInputBase.h"
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "_myheads.h"
#include "dataType.h"

class dataInputPos : public dataInputBase
{
    Q_OBJECT

public:
    dataInputPos();

    void startMpReqTimer();
    void stopMpReqTimer();

    int checkResponseData(const QByteArray &dataIn);
    void dataHandling(const QByteArray &dataIn);
    QByteArray getPsQueryCmdArray(int cmdnum);

private:
    QUdpSocket udpSocket;

    QHostAddress selfAddr{"192.168.200.205"};
    QHostAddress mpAddr{"192.168.200.200"};
    quint16 portNum{16800};

    QTimer *sendTimer;

private slots:
    void msgRecv();
    void msgSend();
};

#endif // DATAMPPOS_H
