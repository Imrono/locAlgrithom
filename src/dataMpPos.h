#ifndef DATAMPPOS_H
#define DATAMPPOS_H
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include "_myheads.h"
#include "dataType.h"

class uiMainWindow;
class dataMpPos : public QObject
{
    Q_OBJECT

public:
    dataMpPos();

    void setParent(uiMainWindow *parent) {
        this->parent = parent;
    }

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

    uiMainWindow *parent;

private slots:
    void msgRecv();
    void msgSend();
};

#endif // DATAMPPOS_H
